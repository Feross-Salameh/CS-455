using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace ProxyServerCS
{
    public class Worker
    {
        public Socket recSok;
        // This method will be called when the thread is started. 
        public void DoWork()
        {
            Console.WriteLine("\nStarting new Thread...");
            while (!_shouldStop)
            {
                byte[] buffer = new byte[80*1024];
                int i = recSok.Receive(buffer);
                Socket conSok = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                // next few lines is to translate message into something more manageble 
                string message = Encoding.UTF8.GetString(buffer);
                string[] lines = message.Split(new string[] { "\r\n" }, StringSplitOptions.None);
                string[] host = lines[1].Split(' ');

                // setting up socket to connect to server...
                var ips = Dns.GetHostEntry(host[1]).AddressList;
                foreach (var ip in ips)
                    if(ip.AddressFamily == AddressFamily.InterNetwork)
                        try
                        {
                            conSok.Connect(ip, 80);
                        }
                        catch
                        {
                            Console.WriteLine("issue with connecting socket.");
                            _shouldStop = true;
                            break;
                        }

                // http 1.1 stuff, removing headers/adding more
                var lLines = lines.ToList();
                for (i = 0; i < lines.Length; i++)
                {
                    if (lLines[i].Contains("Connection:"))
                        lLines.RemoveAt(i);
                    else if ((lines[i].Contains("Proxy-Connection:")))
                        lLines.RemoveAt(i);

                    if (lines[i] == "")
                        lLines.Insert(i - 1, "Connection: close");
                }
                // translate message back.
                buffer = lLines.SelectMany(s => Encoding.ASCII.GetBytes(s)).ToArray();

                while(true) // main send/recv loop stuff. pretty easy to follow.
                    try
                    {
                        conSok.Send(buffer);
                        conSok.Receive(buffer);
                        recSok.Send(buffer);
                        recSok.Receive(buffer);
                        message = Encoding.UTF8.GetString(buffer);
                        lines = message.Split(new string[] { "\r\n" }, StringSplitOptions.None);
                        lLines = lines.ToList();
                        for (i = 0; i < lines.Length; i++)
                        {
                            if (lLines[i].Contains("Connection:"))
                                lLines.RemoveAt(i);
                            else if ((lines[i].Contains("Proxy-Connection:")))
                                lLines.RemoveAt(i);
                            if (lines[i] == "")
                                lLines.Insert(i - 1, "Connection: close");

                        }

                    }
                    catch(Exception e)
                    {
                        Console.WriteLine("error in send/recv cycle: " + e.ToString());
                        break;
                    }

                _shouldStop = true;
                break;
                
            }
            Console.WriteLine("worker thread: terminating gracefully.");
        }
        public void RequestStop()
        {
            _shouldStop = true;
        }
        // Volatile is used as hint to the compiler that this data 
        // member will be accessed by multiple threads. 
        private volatile bool _shouldStop;
    }

    class Program
    {
        
        static void Main(string[] args)
        {
            ArrayList threads = new ArrayList(); // Keep track of threads
            IPAddress addr = IPAddress.Parse("127.0.0.1");
            int port = 1990;
            IPEndPoint proxyServer = new IPEndPoint(addr, port); // address/port of proxy server 
            Socket listenSocket = new Socket(proxyServer.AddressFamily, SocketType.Stream, ProtocolType.Tcp); // socket used to listen for new connections
            try
            {
                // will attempt to bind to local port, should always work
                listenSocket.Bind(proxyServer);
                listenSocket.Listen(100);
                Console.WriteLine("Listening socket now bound to " + proxyServer.ToString());
            }
            catch
            {
                Console.WriteLine("error in bind call, Exiting");
                return;
            }

            while(true)
            {
                // main select loop, The socket.select doesn't hang so I needed to include this loop
                ArrayList sok = new ArrayList();
                sok.Add(listenSocket);
                Socket.Select(sok, null, null, 1000);

                Socket newSok = listenSocket.Accept();
                if(newSok != null)
                {
                    // if you're in here it means that means the accept() function returned a valid socket
                    // stuff for threading...
                    newSok.ReceiveTimeout = 2000;
                    Worker connectionObject = new Worker();
                    connectionObject.recSok = newSok;
                    Thread connectionThread = new Thread(connectionObject.DoWork);
                    threads.Add(connectionThread);
                    connectionThread.Start();
                }


            }
        }
    }
}

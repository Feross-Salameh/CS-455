using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;

namespace command_gui
{
    public partial class starter : Form
    {
        List<Process> routerList = new List<Process>();
        public starter()
        {
            InitializeComponent();
        }

        private void btn_start_Click(object sender, EventArgs e)
        {
            string name = tb_name.Text;
            bool check = cb_poisedReverse.Checked;
        }

        private void btn_startFromFolder_Click(object sender, EventArgs e)
        {
            string name = tb_name.Text;
            string path = "";
            bool check = cb_poisedReverse.Checked;
            using (FolderBrowserDialog fbd = new FolderBrowserDialog())
            {
                fbd.SelectedPath = Directory.GetCurrentDirectory();
                if (fbd.ShowDialog() == DialogResult.OK)
                {
                    path = fbd.SelectedPath;
                }
            }
            Directory.SetCurrentDirectory(path);
            string[] paths = Directory.GetFiles(path);

            foreach(string str in paths)
            {
                string[] tok = str.Split('\\');
                string file = tok.Last();
                string[] fileSplt = file.Split('.');
                if (fileSplt.Last() == "cfg")
                {
                    Process newProc = new Process();
                    newProc.StartInfo.FileName = @"C:\Users\Feross Salameh\Dropbox\CS 455\Projects\CS-455\Distance vector router\router\Debug\router.exe";
                    newProc.StartInfo.WorkingDirectory = "..\\..\\..\\..\\router\\Debug\\";
                    if (check)
                        newProc.StartInfo.Arguments = " -p";
                    newProc.StartInfo.Arguments += " " + tok[tok.Length - 2] + " ";
                    newProc.StartInfo.Arguments += file[0].ToString();
                    newProc.Start();
                    routerList.Add(newProc);
                }
            }
        }
    }
}

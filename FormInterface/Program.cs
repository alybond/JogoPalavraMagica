using System;
using System.Windows.Forms;

namespace FormInterface
{
    internal static class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            string comando = args.Length > 0 ? args[0].ToUpper() : "DEFAULT";
            Application.Run(new Form1(comando));
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace editor
{
    public partial class Form1 : Form
    {
        QzWrapper.Engine engine = new QzWrapper.Engine();

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            engine.Initialize(true, panel1.Handle);
            timer1.Start();



//            typeof(Panel).InvokeMember("DoubleBuffered",
//System.Reflection.BindingFlags.SetProperty | System.Reflection.BindingFlags.Instance | System.Reflection.BindingFlags.NonPublic,
//null, panel1, new object[] { true });

        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            //engine.Update();
            //this.Refresh();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            //this.Invalidate();
            //Console.WriteLine("f");

            engine.Update();
            //panel1.Invalidate();

            //this.Refresh();
        }

        private void Form1_SizeChanged(object sender, EventArgs e)
        {
            
        }

        private async void DoUpdate()
        {
            
        }
    }
}

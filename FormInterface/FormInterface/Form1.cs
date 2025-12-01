using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace FormInterface
{
    public partial class Form1 : Form
    {
        public Form1(string comando)
        {
            InitializeComponent();
            CarregarTela(comando);
        }

        private void CarregarTela(string comando)
        {
            string imagem = "";
            string texto = "";

            switch (comando)
            {
                case "PRINCESA":
                    imagem = "princesas.jpeg";
                    texto = "Tema: PRINCESAS";
                    break;

                case "ANIMAL":
                    imagem = "animais.jpeg";
                    texto = "Tema: ANIMAIS";
                    break;

                case "AVENTURA":
                    imagem = "cavaleiro.jpeg";
                    texto = "Tema: AVENTURA";
                    break;

                case "ACERTO":
                    imagem = "acerto.jpeg";
                    texto = "Você acertou uma letra!";
                    break;

                case "ERRO":
                    imagem = "erro.jpeg";
                    texto = "Letra incorreta...";
                    break;

                case "PARABENS":
                    imagem = "vitoria.jpeg";
                    texto = "Parabéns! Você venceu!";
                    break;

                case "GAMEOVER":
                    imagem = "erro.jpeg";
                    texto = "Fim de jogo!";
                    break;

                default:
                    texto = "Palavra Mágica";
                    break;
            }

            lblMensagem.Text = texto;

            if (!string.IsNullOrEmpty(imagem))
            {
                string caminho = Path.Combine(Application.StartupPath, imagem);
                if (File.Exists(caminho))
                {
                    pictureBox1.Image = Image.FromFile(caminho);
                }
            }
        }
    }
}

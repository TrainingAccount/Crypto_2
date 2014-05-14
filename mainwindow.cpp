#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_radioButton_6_clicked(bool checked)
{
    if (!this->ui->pushButton->isEnabled())
        this->ui->pushButton->setEnabled(true);
    if (checked == true && !this->ui->lineEdit_7->isHidden()) {
        this->ui->lineEdit_7->hide();
        this->ui->label_7->hide();
    }
}

void MainWindow::on_radioButton_5_clicked(bool checked)
{
    if (!this->ui->pushButton->isEnabled())
        this->ui->pushButton->setEnabled(true);
    if (checked == true && this->ui->lineEdit_7->isHidden()) {
        this->ui->lineEdit_7->show();
        this->ui->label_7->show();
    }
}

int MainWindow::lsbEncrypt() {
    QString sInputFileName = this->ui->lineEdit_7->text();
    QString sPicFileName = this->ui->lineEdit_8->text();
    QString sOutputFileName = this->ui->lineEdit_9->text();

    unsigned char pictureByte;
    unsigned char messageByte;

    FILE *f1=fopen(sInputFileName.toLocal8Bit(), "r");
    fseek(f1, 0, SEEK_END);
    int textSize=ftell(f1);
    rewind(f1);

    FILE *f2=fopen(sPicFileName.toLocal8Bit(), "rb");
    fseek(f2, 0, SEEK_END);
    int picSize=ftell(f2);
    rewind(f2);

    if (textSize > (picSize - (BMP_HEADER_SIZE + MAX_SIZE / 2 ))/4 ) {
       cout << "Choosen picture is too small for this message."
            << endl << "Please choose another picture."
            << endl;
       return -1;
    }

        FILE *f3=fopen(sOutputFileName.toLocal8Bit(), "wb");

        for (int i = 0 ; i < BMP_HEADER_SIZE ; i++) {
                pictureByte=fgetc(f2);
        fputc(pictureByte, f3);
        }

        int *textSizeBin = new int[MAX_SIZE];
    int textSize2 = textSize;
    int i =31;

    memset(textSizeBin,0,sizeof(int)*MAX_SIZE);

    while(textSize2!=0) {
        textSizeBin[i--]=textSize2%2;
        textSize2=textSize2/2;
    }

        for ( i = 0 ; i < MAX_SIZE ; i+=2) {
        pictureByte = fgetc(f2);
        pictureByte-=pictureByte%4;
        pictureByte+=textSizeBin[i]*2 + textSizeBin[i+1];
        fputc(pictureByte, f3);
    }

    for( i = 0 ; i < textSize; i++ ) {
        messageByte = fgetc(f1);
        int j,k;
        for( j = 6 , k = 192 ; j >= 0 ; j -= 2, k = k >> 2 ) {
            pictureByte = fgetc(f2);
            pictureByte = pictureByte & 252 | ((messageByte & k) >> j);
            fputc(pictureByte,f3);
        }
    }

        while(!feof(f2)) {
        pictureByte=fgetc(f2);
        fputc(pictureByte, f3);
    }

    fclose(f1);
    fclose(f2);
    fclose(f3);
    return 0;
}

int MainWindow::lsbDecrypt() {
    QString sInputFileName =  ui->lineEdit_8->text();
    QString sOutputFileName = ui->lineEdit_9->text();

    char messageByte;
    char pictureByte;

    FILE *f1=fopen(sInputFileName.toLocal8Bit(), "rb");
    FILE *f2=fopen(sOutputFileName.toLocal8Bit(), "wb");

    for (int i = 0 ; i < BMP_HEADER_SIZE ; i++)
        fgetc(f1);

    int* messageLengthBin = new int[MAX_SIZE];
    for (int i = 0 ; i < MAX_SIZE ; i+=2) {
        pictureByte = fgetc(f1);
        messageLengthBin[i] = (pictureByte & 2)/2 ;
        messageLengthBin[i+1] = pictureByte & 1;
    }

    int messageLength = 0;
    for (int i = 0 ; i < MAX_SIZE-1 ; i++) {
        messageLength += messageLengthBin[i];
        messageLength *= 2;
    }

    messageLength += messageLengthBin[MAX_SIZE-1];

    for (int i = 0 ; i < messageLength ; i++) {
        messageByte = 0;
        for (int j = 0 ; j < 4 ; j++) {
            messageByte = messageByte << 2;
            pictureByte = fgetc(f1);
            messageByte += pictureByte & 3;
        }
        fputc(messageByte, f2);
    }
    fclose(f1);
    fclose(f2);
    return 0;
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->radioButton_5->isChecked()) {
        lsbEncrypt();
    }
    else
        lsbDecrypt();
}

void MainWindow::on_radioButton_clicked()
{
    if (!ui->pushButton_2->isEnabled())
       ui->pushButton_2->setEnabled(true);
}

void MainWindow::on_radioButton_2_clicked()
{
    if (!ui->pushButton_2->isEnabled())
       ui->pushButton_2->setEnabled(true);
}

void MainWindow::on_radioButton_9_clicked()
{
    if (!ui->pushButton_4->isEnabled())
       ui->pushButton_4->setEnabled(true);
}

void MainWindow::on_radioButton_10_clicked()
{
    if (!ui->pushButton_4->isEnabled())
       ui->pushButton_4->setEnabled(true);
}

void MainWindow::on_radioButton_7_clicked()
{
    if (!ui->pushButton_3->isEnabled())
       ui->pushButton_3->setEnabled(true);
}

void MainWindow::on_radioButton_8_clicked()
{
    if (!ui->pushButton_3->isEnabled())
       ui->pushButton_3->setEnabled(true);
}

void MainWindow::on_pushButton_2_clicked()
{
    if (ui->radioButton->isChecked()) {
        desEncrypt();
    }
    else
        desDecrypt();
}


void MainWindow::on_pushButton_4_clicked()
{
    if (ui->radioButton_9->isChecked()) {
        vidjEncrypt();
    }
    else
        vidjDecrypt();
}



void MainWindow::on_pushButton_3_clicked()
{
    if (ui->radioButton_7->isChecked()) {
        caezarEncrypt();
    }
    else
        caezarDecrypt();
}

int MainWindow::desEncrypt() {
    QString sInputFileName = this->ui->lineEdit->text();
    QString sOutputFileName = this->ui->lineEdit_2->text();
    QString keyFileName = this->ui->lineEdit_3->text();

    FILE *f1 = fopen(sInputFileName.toLocal8Bit(), "rb");
    FILE *fkey = fopen(keyFileName.toLocal8Bit(), "rb");
    FILE *f2 = fopen(sOutputFileName.toLocal8Bit(), "wb");

    int KeyReplaceTable[56] =
    {
            57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
            10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
            63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
            14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
    };

    int efunc[48] =
    {
            32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
            8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
            16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
            24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
    };

    int ki_table[48] =
    {
            14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
            23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
            41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
            44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
    };

    int si_table[512] =
    {
            14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
            0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
            4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
            15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,
            15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
            3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
            0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
            13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,
            10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
            13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
            13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
            1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,
            7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
            13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
            10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
            3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,
            2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
            14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
            4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
            11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,
            12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
            10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
            9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
            4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,
            4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
            13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
            1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
            6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,
            13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
            1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
            7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
            2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
    };

    int ip1[64] =
    {
            40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
            38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
            36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
            34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25,
    };

    int perestanovka_p[32] =
    {
            16, 7, 20, 21, 29, 12, 28, 17,
            1, 15, 23, 26, 5, 18, 31, 10,
            2, 8, 24, 14, 32, 27, 3, 9,
            19, 13, 30, 6, 22, 11, 4, 25
    };

    char Key[8];
    fread(Key, 1, 8, fkey);
    fclose(fkey);

    unsigned char KeyBin[64];
    for (int i = 0; i < 8; i++)
    {
            unsigned char temp = 0x80;
            for (int j = 0; j < 8; j++)
            {
                    if ((Key[i] & temp) == 0)
                            KeyBin[8 * i + j] = 0;
                    else
                            KeyBin[8 * i + j] = 1;
                    temp = temp >> 1;
            }
    }

    unsigned char KeyL0[28], KeyR0[28];
    int count = 0;
    unsigned char KeyReplaced[56];
    for (int i = 0; i < 56; i++)
            KeyReplaced[i] = KeyBin[KeyReplaceTable[i] - 1];
    for (int i = 0; i < 28; i++)
    {
            KeyL0[i] = KeyReplaced[i];
            KeyR0[i] = KeyReplaced[i + 28];
    }

    unsigned char KeyL0start[28], KeyR0start[28];
    for (int i = 0; i < 28; i++)
    {
            KeyL0start[i] = KeyL0[i];
            KeyR0start[i] = KeyR0[i];
    }

    fseek(f1, 0, SEEK_END);
    int file_len = ftell(f1);
    int file_lenth = file_len / 8;
    if (file_len % 8 == 0)
            file_lenth--;
    rewind(f1);

    for (int file_lent = 0; file_lent <= file_lenth; file_lent++)
    {
            for (int i = 0; i < 28; i++)
            {
                    KeyL0[i] = KeyL0start[i];
                    KeyR0[i] = KeyR0start[i];
            }
            unsigned char Data[8];
            if (file_len - file_lent * 8 < 8)
            {
                    fread(Data, 1, file_len - file_lent * 8, f1);
                    for (int i = file_len - file_lent * 8; i < 8; i++)
                            Data[i] = 0;
            }
            else
                    fread(Data, 1, 8, f1);

            unsigned char DataBin[64];

            for (int i = 0; i < 8; i++)
            {
                    unsigned char temp = 0x80;
                    for (int j = 0; j < 8; j++)
                    {
                            if ((Data[i] & temp) == 0)
                                    DataBin[8 * i + j] = 0;
                            else
                                    DataBin[8 * i + j] = 1;
                            temp = temp >> 1;
                    }
            }

            unsigned char DataBinReplaced[64];
            count = 0;
            for (int j = 57; j <= 63; j = j + 2)
                    for (int i = j; i >= 0; i = i - 8)
                    {
                            DataBinReplaced[count] = DataBin[i];
                            count++;
                    }
            for (int j = 56; j <= 62; j = j + 2)
                    for (int i = j; i >= 0; i = i - 8)
                    {
                            DataBinReplaced[count] = DataBin[i];
                            count++;
                    }

            unsigned char l0[32], r0[32];
            for (int i = 0; i < 32; i++)
            {
                    l0[i] = DataBinReplaced[i];
                    r0[i] = DataBinReplaced[i + 32];
            }

            for (int q = 1; q <= 16; q++)
            {
                    unsigned char cq[28], dq[28];

                    for (int i = 0; i < 28; i++)
                    {
                            if (i != 27)
                                    cq[i] = KeyL0[i + 1];
                            else
                                    cq[i] = KeyL0[0];
                    }
                    for (int i = 0; i < 28; i++)
                            KeyL0[i] = cq[i];
                    for (int i = 0; i < 28; i++)
                    {
                            if (i != 27)
                                    dq[i] = KeyR0[i + 1];
                            else
                                    dq[i] = KeyR0[0];
                    }
                    for (int i = 0; i < 28; i++)
                            KeyR0[i] = dq[i];

                    if ((q != 1) && (q != 2) && (q != 9) && (q != 16))
                    {
                            for (int i = 0; i < 28; i++)
                            {
                                    if (i != 27)
                                            cq[i] = KeyL0[i + 1];
                                    else
                                            cq[i] = KeyL0[0];
                            }
                            for (int i = 0; i < 28; i++)
                                    KeyL0[i] = cq[i];
                            for (int i = 0; i < 28; i++)
                            {
                                    if (i != 27)
                                            dq[i] = KeyR0[i + 1];
                                    else
                                            dq[i] = KeyR0[0];
                            }
                            for (int i = 0; i < 28; i++)
                                    KeyR0[i] = dq[i];
                    }

                    unsigned char kq[48];
                    unsigned char cqdq[56];
                    for (int i = 0; i < 28; i++)
                    {
                            cqdq[i] = cq[i];
                            cqdq[i + 28] = dq[i];
                    }


                    for (int i = 0; i < 48; i++)
                            kq[i] = cqdq[ki_table[i] - 1];
                    unsigned char erq[48];
                    for (int i = 0; i < 48; i++)
                            erq[i] = r0[efunc[i] - 1];

                    for (int i = 0; i < 48; i++)
                            erq[i] = (erq[i] + kq[i]) % 2;

                    unsigned char rqpre[32];
                    for (int k = 0; k < 8; k++)
                    {
                            int str = 0;
                            if (erq[k * 6] == 1)
                                    str += 2;
                            if (erq[k * 6 + 5] == 1)
                                    str += 1;

                            int col = 0;
                            if (erq[k * 6 + 1] == 1)
                                    col += 8;
                            if (erq[k * 6 + 2] == 1)
                                    col += 4;
                            if (erq[k * 6 + 3] == 1)
                                    col += 2;
                            if (erq[k * 6 + 4] == 1)
                                    col += 1;

                            int si_cur = si_table[k * 64 + str * 16 + col];
                            unsigned char v_rq[4];
                            if (si_cur / 8 == 1)
                                    v_rq[0] = 1;
                            else
                                    v_rq[0] = 0;

                            si_cur = si_cur % 8;
                            if (si_cur / 4 == 1)
                                    v_rq[1] = 1;
                            else
                                    v_rq[1] = 0;

                            si_cur = si_cur % 4;
                            if (si_cur / 2 == 1)
                                    v_rq[2] = 1;
                            else
                                    v_rq[2] = 0;

                            si_cur = si_cur % 2;
                            if (si_cur == 1)
                                    v_rq[3] = 1;
                            else
                                    v_rq[3] = 0;

                            for (int i = 0; i < 4; i++)
                                    rqpre[k * 4 + i] = v_rq[i];
                    }

                    unsigned char rq[32];
                    for (int i = 0; i < 32; i++)
                    {
                            rq[i] = rqpre[perestanovka_p[i] - 1];
                    }

                    unsigned char rq_final[32];
                    for (int i = 0; i < 32; i++)
                            rq_final[i] = (l0[i] + rq[i]) % 2;
                    for (int i = 0; i < 32; i++)
                    {
                            l0[i] = r0[i];
                            r0[i] = rq_final[i];
                    }

            }

            unsigned char l0r0[64];
            for (int i = 0; i < 32; i++)
            {
                    l0r0[i] = r0[i];
                    l0r0[i + 32] = l0[i];
            }
            unsigned char output[64];
            for (int i = 0; i < 64; i++)
                    output[i] = l0r0[ip1[i] - 1];

            unsigned char out[8];
            for (int i = 0; i < 8; i++)
                    out[i] = 0;
            for (int i = 0; i < 8; i++)
            {
                    unsigned char temp = 0x80;
                    for (int j = 0; j < 8; j++)
                    {
                            if (output[i * 8 + j] == 1)
                                    out[i] = (out[i] | temp);
                            temp = temp >> 1;
                    }
            }


            for (int i = 0; i < 8; i++)
                    fprintf(f2, "%c", out[i]);

    }


    fclose(f1);
    fclose(f2);
    return 0;
}

int MainWindow::desDecrypt() {
    QString sInputFileName = this->ui->lineEdit->text();
    QString sOutputFileName = this->ui->lineEdit_2->text();
    QString keyFileName = this->ui->lineEdit_3->text();

    FILE *f1 = fopen(sInputFileName.toLocal8Bit(), "rb");
    FILE *fkey = fopen(keyFileName.toLocal8Bit(), "rb");
    FILE *f2 = fopen(sOutputFileName.toLocal8Bit(), "wb");

    int KeyReplaceTable[56] =
    {
            57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
            10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
            63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
            14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
    };

    int ki_table[48] =
    {
            14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
            23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
            41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
            44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
    };

    int si_table[512] =
    {
            14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
            0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
            4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
            15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,
            15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
            3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
            0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
            13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,
            10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
            13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
            13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
            1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,
            7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
            13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
            10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
            3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,
            2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
            14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
            4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
            11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,
            12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
            10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
            9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
            4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,
            4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
            13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
            1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
            6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,
            13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
            1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
            7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
            2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
    };

    int ip1[64] =
    {
            40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
            38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
            36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
            34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25,
    };

    int perestanovka_p[32] =
    {
            16, 7, 20, 21, 29, 12, 28, 17,
            1, 15, 23, 26, 5, 18, 31, 10,
            2, 8, 24, 14, 32, 27, 3, 9,
            19, 13, 30, 6, 22, 11, 4, 25
    };

    char Key[8];
    fread(Key, 1, 8, fkey);
    fclose(fkey);

    unsigned char KeyBin[64];

    for (int i = 0; i < 8; i++)
    {
            unsigned char temp = 0x80;
            for (int j = 0; j < 8; j++)
            {
                    if ((Key[i] & temp) == 0)
                            KeyBin[8 * i + j] = 0;
                    else
                            KeyBin[8 * i + j] = 1;
                    temp = temp >> 1;
            }
    }

    unsigned char KeyL0[28], KeyR0[28];
    int count = 0;
    unsigned char KeyReplaced[56];
    for (int i = 0; i < 56; i++)
            KeyReplaced[i] = KeyBin[KeyReplaceTable[i] - 1];
    for (int i = 0; i < 28; i++)
    {
            KeyL0[i] = KeyReplaced[i];
            KeyR0[i] = KeyReplaced[i + 28];
    }

    unsigned char KeyL0start[28], KeyR0start[28];
    for (int i = 0; i < 28; i++)
    {
            KeyL0start[i] = KeyL0[i];
            KeyR0start[i] = KeyR0[i];
    }


    fseek(f1, 0, SEEK_END);
    int file_len = ftell(f1);
    int file_lenth = file_len / 8;
    if (file_len % 8 == 0)
            file_lenth--;
    rewind(f1);


    for (int file_lent = 0; file_lent <= file_lenth; file_lent++)
    {
            for (int i = 0; i < 28; i++)
            {
                    KeyL0[i] = KeyL0start[i];
                    KeyR0[i] = KeyR0start[i];
            }

            unsigned char Data[8];
            if (file_len - file_lent * 8 < 8)
            {
                    fread(Data, 1, file_len - file_lent * 8, f1);
                    for (int i = file_len - file_lent * 8; i < 8; i++)
                            Data[i] = 0;
            }
            else
                    fread(Data, 1, 8, f1);

            unsigned char DataBin[64];

            for (int i = 0; i < 8; i++)
            {
                    unsigned char temp = 0x80;
                    for (int j = 0; j < 8; j++)
                    {
                            if ((Data[i] & temp) == 0)
                                    DataBin[8 * i + j] = 0;
                            else
                                    DataBin[8 * i + j] = 1;
                            temp = temp >> 1;
                    }
            }

            unsigned char DataBinReplaced[64];
            count = 0;
            for (int j = 57; j <= 63; j = j + 2)
                    for (int i = j; i >= 0; i = i - 8)
                    {
                            DataBinReplaced[count] = DataBin[i];
                            count++;
                    }
            for (int j = 56; j <= 62; j = j + 2)
                    for (int i = j; i >= 0; i = i - 8)
                    {
                            DataBinReplaced[count] = DataBin[i];
                            count++;
                    }

            unsigned char l0[32], r0[32];
            for (int i = 0; i < 32; i++)
            {
                    r0[i] = DataBinReplaced[i];
                    l0[i] = DataBinReplaced[i + 32];
            }

            for (int q = 1; q <= 16; q++)
            {
                    unsigned char cq[28], dq[28];

                    if (q == 1)
                    {
                            for (int i = 0; i < 28; i++)
                            {
                                    cq[i] = KeyL0[i];
                                    dq[i] = KeyR0[i];
                            }
                    }
                    else
                    {
                            for (int i = 27; i >= 0; i--)
                            {
                                    if (i != 0)
                                            cq[i] = KeyL0[i - 1];
                                    else
                                            cq[i] = KeyL0[27];
                            }
                            for (int i = 0; i < 28; i++)
                                    KeyL0[i] = cq[i];
                            for (int i = 27; i >= 0; i--)
                            {
                                    if (i != 0)
                                            dq[i] = KeyR0[i - 1];
                                    else
                                            dq[i] = KeyR0[27];
                            }
                            for (int i = 0; i < 28; i++)
                                    KeyR0[i] = dq[i];

                            if ((q != 2) && (q != 9) && (q != 16))
                            {
                                    for (int i = 27; i >= 0; i--)
                                    {
                                            if (i != 0)
                                                    cq[i] = KeyL0[i - 1];
                                            else
                                                    cq[i] = KeyL0[27];
                                    }
                                    for (int i = 0; i < 28; i++)
                                            KeyL0[i] = cq[i];
                                    for (int i = 27; i >= 0; i--)
                                    {
                                            if (i != 0)
                                                    dq[i] = KeyR0[i - 1];
                                            else
                                                    dq[i] = KeyR0[27];
                                    }
                                    for (int i = 0; i < 28; i++)
                                            KeyR0[i] = dq[i];
                            }
                    }

                    unsigned char kq[48];
                    unsigned char cqdq[56];
                    for (int i = 0; i < 28; i++)
                    {
                            cqdq[i] = cq[i];
                            cqdq[i + 28] = dq[i];
                    }


                    for (int i = 0; i < 48; i++)
                            kq[i] = cqdq[ki_table[i] - 1];

                    unsigned char elq[48];
                    elq[0] = l0[31];
                    for (int i = 1; i < 6; i++)
                    {
                            elq[i] = l0[i - 1];
                    }
                    for (int j = 1; j <= 6; j++)
                    {
                            for (int i = 6 * j; i < 6 * (j + 1); i++)
                            {
                                    elq[i] = l0[4 * j + (i - 6 * j) - 1];
                            }
                    }
                    for (int i = 42; i <= 46; i++)
                    {
                            elq[i] = l0[i - 15];
                    }
                    elq[47] = l0[0];

                    for (int i = 0; i < 48; i++)
                    {
                            if ((elq[i] + kq[i]) == 2)
                                    elq[i] = 0;
                            else
                                    elq[i] = elq[i] + kq[i];
                    }

                    unsigned char lqpre[32];
                    for (int k = 0; k < 8; k++)
                    {
                            int str = 0;
                            if (elq[k * 6] == 1)
                                    str += 2;
                            if (elq[k * 6 + 5] == 1)
                                    str += 1;
                            int col = 0;
                            if (elq[k * 6 + 1] == 1)
                                    col += 8;
                            if (elq[k * 6 + 2] == 1)
                                    col += 4;
                            if (elq[k * 6 + 3] == 1)
                                    col += 2;
                            if (elq[k * 6 + 4] == 1)
                                    col += 1;
                            int si_cur = si_table[k * 64 + str * 16 + col];
                            unsigned char v_lq[4];
                            if (si_cur / 8 == 1)
                                    v_lq[0] = 1;
                            else
                                    v_lq[0] = 0;
                            si_cur = si_cur % 8;
                            if (si_cur / 4 == 1)
                                    v_lq[1] = 1;
                            else
                                    v_lq[1] = 0;
                            si_cur = si_cur % 4;
                            if (si_cur / 2 == 1)
                                    v_lq[2] = 1;
                            else
                                    v_lq[2] = 0;
                            si_cur = si_cur % 2;
                            if (si_cur == 1)
                                    v_lq[3] = 1;
                            else
                                    v_lq[3] = 0;
                            for (int i = 0; i < 4; i++)
                                    lqpre[k * 4 + i] = v_lq[i];
                    }

                    unsigned char lq[32];
                    for (int i = 0; i < 32; i++)
                    {
                            lq[i] = lqpre[perestanovka_p[i] - 1];
                    }

                    unsigned char lq_final[32];
                    for (int i = 0; i < 32; i++)
                    {
                            if (r0[i] + lq[i] == 2)
                                    lq_final[i] = 0;
                            else
                                    lq_final[i] = r0[i] + lq[i];
                    }
                    for (int i = 0; i < 32; i++)
                    {
                            r0[i] = l0[i];
                            l0[i] = lq_final[i];
                    }

            }

            unsigned char l0r0[64];
            for (int i = 0; i < 32; i++)
            {
                    l0r0[i] = l0[i];
                    l0r0[i + 32] = r0[i];
            }
            unsigned char output[64];
            for (int i = 0; i < 64; i++)
            {
                    output[i] = l0r0[ip1[i] - 1];
            }

            unsigned char out[8];
            for (int i = 0; i < 8; i++)
                    out[i] = 0;
            for (int i = 0; i < 8; i++)
            {
                    unsigned char temp = 0x80;
                    for (int j = 0; j < 8; j++)
                    {
                            if (output[i * 8 + j] == 1)
                                    out[i] = (out[i] | temp);
                            temp = temp >> 1;
                    }
            }


            for (int i = 0; i < 8; i++)
                    fprintf(f2, "%c", out[i]);

    }


    fclose(f1);
    fclose(f2);
    return 0;
}

int MainWindow::vidjEncrypt() {
    QString sInputFileName = this->ui->lineEdit_13->text();
    QString sOutputFileName = this->ui->lineEdit_14->text();
    QString word = this->ui->lineEdit_15->text();
    string key = word.toStdString();
    string result = "";
    string key_on_s = "";
    bool flag;
    int x = 0, y = 0;
    int registr = 0;
    char dublicat;
    string s;


    int shift = 0;
    char **tabula_recta = new char *[26];
    for (int i=0;i<26;i++)
        tabula_recta[i] = new char [26];
    string alfabet = "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < 26; i++)
        for (int j = 0; j < 26; j++)
            {
                shift = j + i;
                if (shift >= 26) shift = shift % 26;
                    tabula_recta[i][j] = alfabet[shift];
            }
    ifstream in(sInputFileName.toStdString().c_str());
    getline(in,s);
    in.close();


    for (unsigned int i = 0; i < s.length(); i++)
        {
            key_on_s += key[i % key.length()];
        }

    for (unsigned int i = 0; i < s.length(); i++)
        {

            if (((int)(s[i]) < 65) || ((int)(s[i]) > 122))
                result += s[i];
            else
                {

                    int l = 0;
                    flag = false;

                    while ((l < 26) && (flag == false))
                    {

                        if (key_on_s[i] == tabula_recta[l][0])
                        {

                            x = l;
                            flag = true;
                        }
                        l++;
                    }


                    if (((int)(s[i]) <= 90) && ((int)(s[i]) >= 65))
                    {
                        dublicat = (char)((int)(s[i]) + 32);
                        registr = 1;
                    }
                    else
                    {
                        registr = 0;
                        dublicat = s[i];
                    }
                    l = 0;
                    flag = false;


                    while ((l < 26) && (flag == false))
                    {
                        if (dublicat == tabula_recta[0][l])
                        {

                            y = l;
                            flag = true;
                        }
                        l++;
                    }

                    if (registr == 1)
                    {

                        dublicat = char((int)(tabula_recta[x][y]) - 32);
                        result += dublicat;
                    }
                    else
                        result += tabula_recta[x][y];
                }
            }

    ofstream out(sOutputFileName.toStdString().c_str());
    out<<result;
    out.close();

    return 0;
}

int MainWindow::vidjDecrypt() {
    QString sInputFileName = this->ui->lineEdit_13->text();
    QString sOutputFileName = this->ui->lineEdit_14->text();
    QString word = this->ui->lineEdit_15->text();
    string stdword = word.toStdString();
    string key = word.toStdString();
    string result = "";
    string key_on_s = "";
    bool flag;
    int x = 0, y = 0;
    int registr = 0;
    char dublicat;
    string s;


    int shift = 0;
    char **tabula_recta = new char *[26];
    for (int i=0;i<26;i++)
        tabula_recta[i] = new char [26];
    string alfabet = "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < 26; i++)
        for (int j = 0; j < 26; j++)
            {
                shift = j + i;
                if (shift >= 26) shift = shift % 26;
                    tabula_recta[i][j] = alfabet[shift];
            }
    ifstream in(sInputFileName.toStdString().c_str());
    getline(in,s);
    in.close();


    for (unsigned int i = 0; i < s.length(); i++)
        {
            key_on_s += key[i % key.length()];
        }

    for (unsigned int i = 0; i < s.length(); i++)
        {

            if (((int)(s[i]) < 65) || ((int)(s[i]) > 122))
                result += s[i];
            else
                {

                    int l = 0;
                    flag = false;

                    while ((l < 26) && (flag == false))
                    {

                        if (key_on_s[i] == tabula_recta[l][0])
                        {

                            x = l;
                            flag = true;
                        }
                        l++;
                    }

                    if (((int)(s[i]) <= 90) && ((int)(s[i]) >= 65))
                    {
                        dublicat = (char)((int)(s[i]) + 32);
                        registr = 1;
                    }
                    else
                    {
                        registr = 0;
                        dublicat = s[i];
                    }
                    l = 0;
                    flag = false;

                    while ((l < 26) && (flag == false))
                    {
                        if (dublicat == tabula_recta[x][l])
                        {

                            y = l;
                            flag = true;
                        }
                        l++;
                    }

                    if (registr == 1)
                    {

                        dublicat = char((int)(tabula_recta[0][y]) - 32);
                        result += dublicat;
                    }
                    else
                        result += tabula_recta[0][y];
                }
            }

    ofstream out(sOutputFileName.toStdString().c_str());
    out<<result;
    out.close();

    return 0;
}

int MainWindow::caezarEncrypt() {
    QString sInputFileName = this->ui->lineEdit_10->text();
    QString sOutputFileName = this->ui->lineEdit_11->text();
    QString qshift = this->ui->lineEdit_12->text();
    int shift = qshift.toInt();

    string result = "";
    string s;
    ifstream in(sInputFileName.toStdString().c_str());
    getline(in,s);
    in.close();

    if (shift > 26)
        shift = shift % 26;

        for (unsigned int i = 0; i < s.length(); i++)
        {
            if (((int)(s[i]) < 65)||((int)(s[i]) > 122))
                result += s[i];

            if (((int)(s[i]) >= 97) && ((int)(s[i]) <= 122))
                {
                        if ((int)(s[i]) + shift > 122)
                            result += (char)((int)(s[i]) + shift - 26);
                        else
                            result += (char)((int)(s[i]) + shift);
                 }

                 if (((int)(s[i]) >= 65) && ((int)(s[i]) <= 90))
                    {

                        if ((int)(s[i]) + shift > 90)
                            result += (char)((int)(s[i]) + shift - 26);
                        else
                            result += (char)((int)(s[i]) + shift);
                     }
        }
    ofstream out(sOutputFileName.toStdString().c_str());
    out<<result;
    out.close();

    return 0;
}

int MainWindow::caezarDecrypt() {
    QString sInputFileName = this->ui->lineEdit_10->text();
    QString sOutputFileName = this->ui->lineEdit_11->text();
    QString qshift = this->ui->lineEdit_12->text();
    int shift = qshift.toInt();

    string result = "";
    string s;
    ifstream in(sInputFileName.toStdString().c_str());
    getline(in,s);
    in.close();

    if (shift > 26)
        shift = shift % 26;

    for (unsigned int i = 0; i < s.length(); i++)
    {

        if (((int)(s[i]) < 65)||((int)(s[i]) > 122))
            result += s[i];

        if (((int)(s[i]) >= 97) && ((int)(s[i]) <= 122))
        {

            if ((int)(s[i]) - shift < 97)
               result += (char)((int)(s[i]) - shift + 26);
            else
               result += (char)((int)(s[i]) - shift);
        }

        if (((int)(s[i]) >= 65) && ((int)(s[i]) <= 90))
        {

           if ((int)(s[i]) - shift < 65)
              result += (char)((int)(s[i]) - shift + 26);
           else
              result += (char)((int)(s[i]) - shift);
        }
     }
     ofstream out(sOutputFileName.toStdString().c_str());
     out<<result;
     out.close();

    return 0;
}

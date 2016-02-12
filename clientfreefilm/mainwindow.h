#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QtWebKitWidgets/QWebView>

#include<QUrl>
#include<QWebSettings>
#include<QWebFrame>
#include<QWebPage>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QVariant>
#include<QFile>
#include<QString>
#include<QMessageBox>
#include<QByteArray>
#include<QDebug>
#include<QEventLoop>
#include<QInputDialog>
#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QStringList>
#include<QRegExp>
#include<QDesktopServices>




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
 QWebView *wb1, *wb2, *wb3, *wb4;
 QDialog *dial, *dial2;
 QPushButton *pb;
 QVBoxLayout *VBox, *VBox2;
 QHBoxLayout *HBox, *HBox2;
 QVariant mes;
 QString string, Url1,Url2,ServIP, PosterURL,script,jQuery,ans, Html,Title,images, imgurl;
 QString ExVideo,ExFormat,ExCodec,ExQval,ExTrueVideo,ExSound,ExTrueSound,ExTranslation,ExSub,ExFileDown;
 QStringList ExIm1;
 QString FileName,GenrFilm[2][52],qval[21],codec[10],format[9],sound[13],Descr,transl[12];
 QString KpRuName,KpEngName,KpPosterURL,KpId, KpYear,KpDescription, KpCountry, KpDirector, KpCasting, KpScene, KpBudget,
 KpDateWorld, KpSlogan,KpTime,KpKnows,KpGenrFilm, KpDateRF, KpDvd;

 bool done,done2;
private slots:
 void on_pushButton_clicked();
 void replyFinished(QNetworkReply*);
 void on_pushButton_2_clicked();
 void on_pushButton_3_clicked();
 void Phase2Do(QString);
 void on_pushButton_7_clicked();
 void loadFinished(bool);
 void on_pushButton_4_clicked();
 void UIloadFinished(bool);
 void ArrayInit();
 void changeProgress(int);
 void loadFinished2(bool);
 void pb_click(bool);
 void FileDownloader(QNetworkReply*);
 void img_put(QString);
 void on_pushButton_6_clicked();

 void on_pushButton_5_clicked();

signals:
void phase2(QString);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ArrayInit();

    QFile file;//загрузка jquery и добавление ноу конфликт
    file.setFileName(":/jquery-1.11.3.min.js");
    file.open(QIODevice::ReadOnly);
    jQuery = file.readAll();
    jQuery.append("\nvar qt = { 'jQuery': jQuery.noConflict(true) };");
    file.close();

    ServIP="127.0.0.1";


    QFile htmlF;//подгрузка html
    htmlF.setFileName(":/page.html");
    htmlF.open(QFile::ReadOnly);
    Html = htmlF.readAll();
    htmlF.close();

    QFile styleF;//подгрузка стилей
    styleF.setFileName("://style.qss");
    styleF.open(QFile::ReadOnly);
    QString qssStr = styleF.readAll();
    qApp->setStyleSheet(qssStr);
    styleF.close();

     QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
     QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, true);
     QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
     connect(ui->webView,SIGNAL(loadFinished(bool)),this,SLOT(UIloadFinished(bool)));
     ui->webView->load(QUrl("http://free-filmy.ru/"));

    connect(this,SIGNAL (phase2(QString)),this ,SLOT(Phase2Do(QString)));
    wb1=new QWebView();
    connect(wb1,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished(bool)));
    connect(wb1,SIGNAL(loadProgress(int)),this, SLOT(changeProgress(int)));
    wb2=new QWebView();
    connect(wb2,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished2(bool)));

    dial=new QDialog;
    wb3=new QWebView();
    pb=new QPushButton();
    pb->setText("OK");
    VBox=new QVBoxLayout;
    VBox->addWidget(wb3);
    VBox->addWidget(pb);
    HBox=new QHBoxLayout;
    HBox->addLayout(VBox);
    dial->setLayout(HBox);

    dial2=new QDialog;
    wb4=new QWebView();
    VBox2=new QVBoxLayout;
    VBox2->addWidget(wb4);
    HBox2=new QHBoxLayout;
    HBox2->addLayout(VBox2);
    dial2->setLayout(HBox2);


  connect(pb,SIGNAL(clicked(bool)),this,SLOT(pb_click(bool)));
  connect(wb3->page()->mainFrame(),SIGNAL(titleChanged(QString)),this,SLOT(img_put(QString)));

  done2=true;

}
//-------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}
//-------------------------------------------------------------------------------
void MainWindow::on_pushButton_clicked()//авторизация
{
    //проверка урл
    if(ui->webView->page()->mainFrame()->url().toString().startsWith("http://free-filmy.ru/")!=true)
    {
    QMessageBox *nm=new QMessageBox;
    nm->setText("не та страница");
    nm->exec();
    delete nm;
    return;
    }
   // проверка авторизации
    if(ui->webView->page()->mainFrame()->toHtml().indexOf("Мой профиль")!=-1 && ui->webView->page()->mainFrame()->toHtml().indexOf("http://free-filmy.ru/user/")!=-1 )
    {
        QMessageBox *nm=new QMessageBox;
        nm->setText("Авторизация уже произошла");
        nm->exec();
        delete nm;
        return;
    }

    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByName('login_name')[0].value='zhosan'");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByName('login_password')[0].value='270283'");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByName('image')[0].click()");
}
//-------------------------------------------------------------------------------
void MainWindow::replyFinished(QNetworkReply *reply)//ответ от сервера фильмов
{
//окончание запроса на сервер и прием данных
    if (reply->error() != QNetworkReply::NoError)    {

         // обрабатываем ошибку
        QMessageBox *nm=new QMessageBox;
        nm->setText(reply->errorString());
        nm->exec();
        delete nm;
        reply->deleteLater();
        return;
       }
    else     {
      // если нет ошибки Читаем ответ от сервера
     QByteArray bytes = reply->readAll();
     string=(QUrl::fromPercentEncoding(bytes));

    }//переходим ко второй фазе
    emit phase2(string);
 }
//-------------------------------------------------------------------------------
void MainWindow::on_pushButton_2_clicked()//заполнение
{
    //проверка урл
    if(ui->webView->page()->mainFrame()->url().toString()!="http://free-filmy.ru/addnew.html")
    {
        QMessageBox *nm=new QMessageBox;
        nm->setText("Перейдите на страницу добавления фильмов");
        nm->exec();
        delete nm;
        return;
    }
    setWindowTitle("получаем фильм ");

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("http://"+ServIP+":3540/?dajfilm=true")));

     //подготовка формы
     ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('category').selectedIndex=-1");
     ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByTagName('select')[10].selectedIndex=1");
     ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('load_pc').style.display='none'");
     ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('load_url').style.display=''");
     ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByTagName('select')[0].selectedIndex=1");
     ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('l_pc').style.display='none'");
     ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('l_url').style.display=''");
     ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.screen.value=''");
}
//-------------------------------------------------------------------------------
void MainWindow::on_pushButton_3_clicked()//проверка
{
    bool ok;

    QString text = QInputDialog::getText(this, tr("Настройки"),
                                         tr("Введите скрипт:"), QLineEdit::Normal,
                                         script, &ok);
    if (ok && !text.isEmpty()){

        script=text;
       QVariant txt=ui->webView->page()->mainFrame()->evaluateJavaScript(script);

qDebug()<<txt.toStringList();
    }
}
//-------------------------------------------------------------------------------
void MainWindow::Phase2Do(QString url)
{
    FileName=url.mid(0,url.lastIndexOf("!")-2);
    Url1=url.mid(url.lastIndexOf("!")+1, url.length()-url.lastIndexOf("!")+1);
    setWindowTitle(windowTitle()+FileName);
    wb1->load(QUrl("http://www.kinopoisk.ru/index.php?first=yes&kp_query="+QUrl::toPercentEncoding(FileName)));
}
//-------------------------------------------------------------------------------
void MainWindow::on_pushButton_7_clicked()//настройка ip сервера
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Настройки"),
                                         tr("Введите IP сервера:"), QLineEdit::Normal,
                                         ServIP, &ok);
    if (ok && !text.isEmpty()) ServIP=text;
}
//-------------------------------------------------------------------------------
void MainWindow::loadFinished(bool)
{//окончание загрузки кинопоиска
    wb1->page()->mainFrame()->evaluateJavaScript(jQuery);

 //русское название
  KpRuName=wb1->page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('moviename-big')[0].innerText").toString();
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.rutitle.value='"+KpRuName+"'");

 //английское название
  KpEngName=wb1->page()->mainFrame()->evaluateJavaScript("document.querySelector('[itemprop=\"alternativeHeadline\"]').innerText").toString();
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.entitle.value='"+KpEngName+"'");

 //постер
 KpPosterURL="http://st.kp.yandex.net"+wb1->page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('popupBigImage cloud-zoom')[0].getAttribute('href')").toString();
 ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.poster_url.value='"+KpPosterURL+"'");

 //Id кинопоиска
 Url2=wb1->page()->mainFrame()->url().toString();
 if(Url2.endsWith("/"))Url2.resize(Url2.length()-1);
 KpId=Url2.mid(Url2.lastIndexOf("/")+1, Url2.length()-Url2.lastIndexOf("/")+1);
 ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByName('xfield[kp]')[0].value='"+KpId+"'");

 //описание кинопоиска
KpDescription=wb1->page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('brand_words')[0].innerHTML").toString();
while(KpDescription.indexOf("&nbsp")!=-1)KpDescription.replace("&nbsp"," ");
while(KpDescription.indexOf("\\")!=-1)KpDescription.replace("\\","'");
while(KpDescription.indexOf(";")!=-1)KpDescription.replace(";"," ");
while(KpDescription.indexOf("&#133")!=-1)KpDescription.replace("&#133"," ");
while(KpDescription.indexOf("&#155")!=-1)KpDescription.replace("&#155",".");
while(KpDescription.indexOf("&laquo")!=-1)KpDescription.replace("&laquo","<<");
while(KpDescription.indexOf("&raquo")!=-1)KpDescription.replace("&raquo",">>");
while(KpDescription.indexOf("<br>")!=-1)KpDescription.remove("<br>");
while(KpDescription.indexOf("<\br>")!=-1)KpDescription.remove("<\br>");
while(KpDescription.indexOf("&#151")!=-1)KpDescription.replace("&#151","—");
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.description.value='"+KpDescription+"'");

//год кинопоиска
KpYear=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'год\\')').next().text()").toString();
while(KpYear.indexOf("\n")!=-1)KpYear.remove("\n");
while(KpYear.indexOf(" ")!=-1)KpYear.remove(" ");
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.year.value='"+KpYear+"'");

//страна кинопоиск
KpCountry=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'страна\\')').next().text()").toString();
while(KpCountry.indexOf("\n")!=-1)KpCountry.remove("\n");
while(KpCountry.indexOf(" ")!=-1)KpCountry.remove(" ");
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.country.value='"+KpCountry+"'");

//режисер кинопоиск
KpDirector=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'режиссер\\')').next().text()").toString();
while(KpDirector.indexOf("\n")!=-1)KpDirector.remove("\n");
while(KpDirector.startsWith(" "))KpDirector.remove(" ");
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.director.value='"+KpDirector+"'");

//Актеры кинопоиск
KpCasting=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('h4:contains(\\'В главных ролях:\\')').next().text()").toString();
for(int i=0;i<KpCasting.length()-2;i++){
if(KpCasting.at(i).isLower()&&KpCasting.at(i+1).isUpper())KpCasting.insert(i+1,",");
}
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.casting.value='"+KpCasting+"'");

//Сценарий кинопоиск
KpScene=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'сценарий\\')').next().text()").toString();
while(KpScene.indexOf("\n")!=-1)KpScene.replace("\n"," ");
while(KpScene.startsWith(" "))KpScene.remove(" ");
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.scene.value='"+KpScene+"'");

//бюджет кинопоиск
KpBudget=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'бюджет\\')').next().text()").toString();
while(KpBudget.indexOf("\n")!=-1)KpBudget.remove("\n");
while(KpBudget.startsWith(" "))KpBudget.remove(" ");
while(KpBudget.endsWith(" "))KpBudget.chop(1);
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.buget.value='"+KpBudget+"'");

//премьера мир кинопоиск
KpDateWorld=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'премьера (мир)\\')').next().text()").toString();
while(KpDateWorld.indexOf("\n")!=-1)KpDateWorld.remove("\n");
while(KpDateWorld.startsWith("  "))KpDateWorld.remove("  ");
while(KpDateWorld.endsWith(" "))KpDateWorld.chop(1);
while(KpDateWorld.indexOf("...")!=-1)KpDateWorld.chop(5);
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.date_w.value='"+KpDateWorld+"'");

//премьера Рф
KpDateRF=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'премьера (РФ)\\')').next().text()").toString();
while(KpDateRF.indexOf("\n")!=-1)KpDateRF.remove("\n");
while(KpDateRF.startsWith("  "))KpDateRF.remove("  ");
while(KpDateRF.endsWith(" "))KpDateRF.chop(1);
while(KpDateRF.indexOf("...")!=-1)KpDateRF.chop(5);
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.date_r.value='"+KpDateRF+"'");

//кинопоиск слоган
KpSlogan=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'слоган\\')').next().text()").toString();
//while(KpSlogan.indexOf("\n")!=-1)KpSlogan.remove("\n");
//while(KpSlogan.indexOf("  "))KpSlogan.remove("  ");
if(KpSlogan!="-")ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.slogan.value=\""+KpSlogan+"\"");

//кинопоиск продолжительность
KpTime=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'время\\')').next().text()").toString();
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.time.value='"+KpTime.mid(KpTime.indexOf("/")+1,KpTime.length()-KpTime.indexOf("/")-1)+":00'");

//кинопоиск интересные факты
KpKnows=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('.trivia_text').text()").toString();
while(KpKnows.indexOf("&nbsp")!=-1)KpKnows.replace("&nbsp"," ");
while(KpKnows.indexOf("\\")!=-1)KpKnows.replace("\\","'");
while(KpKnows.indexOf(";")!=-1)KpKnows.replace(";"," ");
while(KpKnows.indexOf("&#133")!=-1)KpKnows.replace("&#133"," ");
while(KpKnows.indexOf("&#155")!=-1)KpKnows.replace("&#155",".");
while(KpKnows.indexOf("&laquo")!=-1)KpKnows.replace("&laquo","<<");
while(KpKnows.indexOf("&raquo")!=-1)KpKnows.replace("&raquo",">>");
while(KpKnows.indexOf("<br>")!=-1)KpKnows.remove("<br>");
while(KpKnows.indexOf("<\br>")!=-1)KpKnows.remove("<\br>");
while(KpKnows.indexOf("&#151")!=-1)KpKnows.replace("&#151","—");
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.knows.value=\""+KpKnows+"\"");

//жанр фильма
KpGenrFilm=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'жанр\\')').next().text()").toString();
for(int q=0;q<52;q++){
if(KpGenrFilm.indexOf(GenrFilm[0][q])!=-1)ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.category.childNodes["+GenrFilm[1][q]+"].setAttribute('selected','selected')");
}
if(KpCountry.indexOf("ссср")!=-1||KpCountry.indexOf("СССР")!=-1||KpCountry.indexOf("Ссср")!=-1)ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.category.childNodes["+GenrFilm[1][30]+"].setAttribute('selected', 'selected')");
if(KpCountry.indexOf("Россия")!=-1||KpCountry.indexOf("росия")!=-1||KpCountry.indexOf("Росия")!=-1||KpCountry.indexOf("росия")!=-1)ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.category.childNodes["+GenrFilm[1][23]+"].setAttribute('selected','selected')");

//релиз на двд
KpDvd=wb1->page()->mainFrame()->evaluateJavaScript("qt.jQuery('td.type:contains(\\'релиз на DVD\\')').next().text()").toString();
KpDvd=KpDvd.simplified();
while(KpDvd.indexOf("&nbsp")!=-1)KpDvd.replace("&nbsp"," ");
if(KpDvd.indexOf("...")!=-1)KpDvd.chop(5);
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.date_dvd.value='"+KpDvd+"'");

//загрузка ex.ua
 wb2->load(QUrl(Url1));
}
//-------------------------------------------------------------------------------

void MainWindow::on_pushButton_4_clicked()//Заполнение без обновления
{

    //подготовка формы
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('category').selectedIndex=-1");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByTagName('select')[10].selectedIndex=1");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('load_pc').style.display='none'");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('load_url').style.display=''");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByTagName('select')[0].selectedIndex=1");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('l_pc').style.display='none'");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementById('l_url').style.display=''");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.screen.value=''");

  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.rutitle.value='"+KpRuName+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.entitle.value='"+KpEngName+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.poster_url.value='"+KpPosterURL+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByName('xfield[kp]')[0].value='"+KpId+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.description.value='"+KpDescription+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.year.value='"+KpYear+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.country.value='"+KpCountry+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.director.value='"+KpDirector+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.casting.value='"+KpCasting+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.scene.value='"+KpScene+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.buget.value='"+KpBudget+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.date_w.value='"+KpDateWorld+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.date_r.value='"+KpDateRF+"'");
  if(KpSlogan!="-")ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.slogan.value=\""+KpSlogan+"\"");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.time.value='"+KpTime.mid(KpTime.indexOf("/")+1,KpTime.length()-KpTime.indexOf("/")-1)+":00'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.knows.value=\""+KpKnows+"\"");

  for(int q=0;q<52;q++){
  if(KpGenrFilm.indexOf(GenrFilm[0][q])!=-1)ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.category.childNodes["+GenrFilm[1][q]+"].setAttribute('selected','selected')");
  }
  if(KpCountry.indexOf("ссср")!=-1||KpCountry.indexOf("СССР")!=-1||KpCountry.indexOf("Ссср")!=-1)ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.category.childNodes["+GenrFilm[1][30]+"].setAttribute('selected', 'selected')");
  if(KpCountry.indexOf("Россия")!=-1||KpCountry.indexOf("росия")!=-1||KpCountry.indexOf("Росия")!=-1||KpCountry.indexOf("росия")!=-1)ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.category.childNodes["+GenrFilm[1][23]+"].setAttribute('selected','selected')");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.date_dvd.value='"+KpDvd+"'");

  dial->show();

  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.format.selectedIndex="+ExFormat);
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.video_codec.selectedIndex="+ExCodec);
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.quality.selectedIndex="+ExQval);

  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.dvideo.value='"+ExTrueVideo+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.audio_codec.selectedIndex="+ExSound);
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.daudio.value='"+ExTrueSound+"'");
  ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.translation.selectedIndex="+ExTranslation);


}
//-------------------------------------------------------------------------------
void MainWindow::UIloadFinished(bool)//загрузка jquery
{
//    ui->webView->page()->mainFrame()->evaluateJavaScript(jQuery);
}
//-------------------------------------------------------------------------------
void MainWindow::ArrayInit()//инициализация массивов
{

    transl[0]="одноголосый";
    transl[1]="двухголосый";
    transl[2]="гоблинa";
    transl[3]="профессиональный (одноголосый)";
    transl[4]="профессиональный (двухголосый)";
    transl[5]="многоголосый";
    transl[6]="полное дублирование";
    transl[7]="ts";
    transl[8]="camrip";
    transl[9]="субтитры";
    transl[10]="отсутствует";
    transl[11]="не требуется";

//==================================================================
    sound[0]="mp3";
    sound[1]="ape";
    sound[2]="flac";
    sound[3]="wav";
    sound[4]="wma";
    sound[5]="ogg";
    sound[6]="dts";
    sound[7]="dvd-audio";
    sound[8]="tta";
    sound[9]="aac";
    sound[10]="ac3";
    sound[11]="m4a";
    sound[12]="m4b";
//========================================================
    GenrFilm[0][0]="трейл";GenrFilm[1][0]="0";
    GenrFilm[0][1]="аним";GenrFilm[1][1]="1";
    GenrFilm[0][2]="артхаус";GenrFilm[1][2]="2";
    GenrFilm[0][3]="боев";GenrFilm[1][3]="3";
    GenrFilm[0][4]="вестерн";GenrFilm[1][4]="4";
    GenrFilm[0][5]="видеоклип";GenrFilm[1][5]="5";
    GenrFilm[0][6]="воен";GenrFilm[1][6]="6";
    GenrFilm[0][7]="детект";GenrFilm[1][7]="7";
    GenrFilm[0][8]="детск";GenrFilm[1][8]="8";
    GenrFilm[0][9]="кпк";GenrFilm[1][9]="9";
    GenrFilm[0][10]="документ";GenrFilm[1][10]="10";
    GenrFilm[0][11]="драм";GenrFilm[1][11]="11";
    GenrFilm[0][12]="истор";GenrFilm[1][12]="12";
    GenrFilm[0][13]="катастроф";GenrFilm[1][13]="13";
    GenrFilm[0][14]="квн";GenrFilm[1][14]="14";
    GenrFilm[0][15]="комед";GenrFilm[1][15]="15";
    GenrFilm[0][16]="короткометраж";GenrFilm[1][16]="16";
    GenrFilm[0][17]="криминал";GenrFilm[1][17]="17";
    GenrFilm[0][18]="мелодрам";GenrFilm[1][18]="18";
    GenrFilm[0][19]="мистик";GenrFilm[1][19]="19";
    GenrFilm[0][20]="мульт";GenrFilm[1][20]="20";
    GenrFilm[0][21]="мюзикл";GenrFilm[1][21]="21";
    GenrFilm[0][22]="не для детей";GenrFilm[1][22]="22";
    GenrFilm[0][23]="отечественные";GenrFilm[1][23]="23";
    GenrFilm[0][24]="охот";GenrFilm[1][24]="24";
    GenrFilm[0][25]="прикл";GenrFilm[1][25]="25";
    GenrFilm[0][26]="реалити-шоу";GenrFilm[1][26]="26";
    GenrFilm[0][27]="ретро";GenrFilm[1][27]="27";
    GenrFilm[0][28]="семей";GenrFilm[1][28]="28";
    GenrFilm[0][29]="сериал";GenrFilm[1][29]="29";
    GenrFilm[0][30]="советское кино";GenrFilm[1][30]="30";
    GenrFilm[0][31]="спектакл";GenrFilm[1][31]="31";
    GenrFilm[0][32]="спорт";GenrFilm[1][32]="32";
    GenrFilm[0][33]="бокс";GenrFilm[1][33]="33";
    GenrFilm[0][34]="футбол";GenrFilm[1][34]="34";
    GenrFilm[0][35]="теле";GenrFilm[1][35]="35";
    GenrFilm[0][36]="трил";GenrFilm[1][36]="36";
    GenrFilm[0][37]="ужас";GenrFilm[1][37]="37";
    GenrFilm[0][38]="фантаст";GenrFilm[1][38]="38";
    GenrFilm[0][39]="фэнтез";GenrFilm[1][39]="39";
    GenrFilm[0][40]="домашн";GenrFilm[1][40]="2";
    GenrFilm[0][41]="концерт";GenrFilm[1][41]="5";
    GenrFilm[0][42]="смартфон";GenrFilm[1][42]="9";
    GenrFilm[0][43]="psp";GenrFilm[1][43]="9";
    GenrFilm[0][44]="биограф";GenrFilm[1][44]="12";
    GenrFilm[0][45]="юмор";GenrFilm[1][45]="14";
    GenrFilm[0][46]="муз";GenrFilm[1][46]="21";
    GenrFilm[0][47]="для взрослых";GenrFilm[1][47]="22";
    GenrFilm[0][48]="рыб";GenrFilm[1][48]="24";
    GenrFilm[0][49]="тв-шоу";GenrFilm[1][49]="35";
    GenrFilm[0][50]="фентез";GenrFilm[1][50]="39";
    GenrFilm[0][51]="сказ";GenrFilm[1][51]="39";;
 //==========================================================================
     qval[0]="DVDRip";
     qval[1]="DVD5";
     qval[2]="DVD5 (сжатый)";
     qval[3]="DVD9";
     qval[4]="DVDScreener";
     qval[5]="HDTV";
     qval[6]="HDRip";
     qval[7]="HDTVRip";
     qval[8]="CamRip";
     qval[9]="TVRip";
     qval[10]="TeleCine";
     qval[11]="TeleSynch";
     qval[12]="SATRip";
     qval[13]="WebRip";
     qval[14]="WEB-DLRip";
     qval[15]="VHSRip";
     qval[16]="HD-DVDRip";
     qval[17]="BDRip";
     qval[18]="720p";
     qval[19]="1080p";
     qval[20]="ТРЕЙЛЕР";
 //========================================================================
    codec[0]="divx";
    codec[1]="xvid";
    codec[2]="mpeg-4";
    codec[3]="vpx";
    codec[4]="mpeg1";
    codec[5]="mpeg2";
    codec[6]="windows media";
    codec[7]="quicktime";
    codec[8]="h.264";
    codec[9]="flash";
 //=========================================================================
    format[0]="avi";
    format[1]="dvd";
    format[2]="ogm";
    format[3]="mkv";
    format[4]="wmv";
    format[5]="mpeg";
    format[6]="mp4";
    format[7]="ts";
    format[8]="m2ts";

}
//-------------------------------------------------------------------------------

void MainWindow::changeProgress(int pr)
{
    setWindowTitle("Получаем фильм "+FileName+" "+QString::number(pr)+"%");
}
//-------------------------------------------------------------------------------
void MainWindow::loadFinished2(bool)//обработка ex.ua
{
    ExFileDown="";
    done2=true;

    ExIm1.clear();
    ExTrueVideo.clear();
    ExTrueSound.clear();
         wb3->page()->mainFrame()->setHtml("");
         wb2->page()->mainFrame()->evaluateJavaScript(jQuery);
         wb3->page()->mainFrame()->setHtml(Html);


    //скриншоты
  for(int i=0;i<10;i++){
  imgurl=wb2->page()->mainFrame()->evaluateJavaScript("document.getElementById('picture_"+QString::number(i)+"').getAttribute('href')").toString();
  imgurl.chop(imgurl.length()-imgurl.indexOf("?")-1);
  ExIm1<<imgurl;
  wb3->page()->mainFrame()->evaluateJavaScript("document.getElementById('"+QString::number(i)+"').setAttribute('src','"+ExIm1.at(i)+"')");
  }
  QString wort=ExIm1.at(5);
  if(wort.isEmpty()||wort.isNull()){
      QDesktopServices::openUrl(QUrl("https://www.google.com.ua/#q="+KpRuName));
      done2=false;
  }
  else {
   done2=true;
   dial->show();
  }


//видео качество
ExVideo=wb2->page()->mainFrame()->evaluateJavaScript("qt.jQuery('p:contains(\\'Видео\\')').text()").toString();
if(ExVideo.isEmpty())ExVideo=wb2->page()->mainFrame()->evaluateJavaScript("qt.jQuery('p:contains(\\'Video\\')').text()").toString();
if(ExVideo.isEmpty())ExVideo=wb2->page()->mainFrame()->evaluateJavaScript("qt.jQuery('p:contains(\\'Відео\\')').text()").toString();
for (int i=0;i<21;i++)
{
    if(ExVideo.contains(qval[i]))
    {
    ExQval=QString::number(i+1);
    break;
    }
}

ExVideo=ExVideo.toLower();

//видео кодек
for (int vc=0;vc<10;vc++)
{
    if(ExVideo.contains(codec[vc]))
    {
     ExCodec=QString::number(vc+1);
     break;
    }
}
//видео формат
for (int vf=0;vf<8;vf++)
{
    if(ExVideo.indexOf(format[vf])!=-1)
    {
     ExFormat=QString::number(vf+1);

     break;
    }
}
if(ExCodec=="2"||ExCodec=="1")ExFormat="1";
if(ExCodec.isEmpty()&&ExFormat=="1")ExCodec="2";
if(ExQval.isEmpty()&&ExFormat=="1")ExQval="1";
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.format.selectedIndex="+ExFormat);
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.video_codec.selectedIndex="+ExCodec);
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.quality.selectedIndex="+ExQval);


//описание видео
QRegExp video1("\\d+[x]\\d+");
//video1.setMinimal(true);
QRegExp video2("\\(\\d(.)\\d\\d?(:)\\d\\)");
video2.setMinimal(true);
QRegExp video3("\\d+\\.\\d+\\s?(fps|кадр/с)");
//video3.setMinimal(true);
QRegExp video4("\\d+\\s?(.)?\\s?\\d{3,}\\s?(бит/с|kbps|кб/с|кбит/с|kb/s)");
//video4.setMinimal(true);
QRegExp video5("\\d+\\.\\d+\\s?(bit/pixel)");
//video5.setMinimal(true);


int Pos=video1.indexIn(ExVideo);
if (Pos>-1)ExTrueVideo+=video1.cap()+" ";
Pos=video2.indexIn(ExVideo);
if (Pos>-1)ExTrueVideo+=video2.cap()+" ";
Pos=video3.indexIn(ExVideo);
if (Pos>-1)ExTrueVideo+=video3.cap()+" ";
Pos=video4.indexIn(ExVideo);
if (Pos>-1)ExTrueVideo+=video4.cap()+" ";
Pos=video5.indexIn(ExVideo);
if (Pos>-1)ExTrueVideo+=video5.cap() +" ";
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.dvideo.value='"+ExTrueVideo+"'");

//формат аудио
for(int ss=0;ss<13;ss++){
if(ExVideo.indexOf(sound[ss])!=-1){
     ExSound=QString::number(ss+1);
     break;
}
}
if(ExSound.isEmpty()&&ExVideo.indexOf("mpeg layer")!=-1)ExSound=QString::number(1);
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.audio_codec.selectedIndex="+ExSound);

//описание аудио
QRegExp audio1("\\s\\d+(.)?\\d+\\s?(khz|кгц)");
audio1.setMinimal(false);

QRegExp audio2("\\s\\d+\\s?(.)?\\s?\\d{0,}\\s?(бит/с|kbps|кб/с|кбит/с|kb/s)");
audio2.setMinimal(false);

Pos=audio1.lastIndexIn(ExVideo);
if (Pos>-1)ExTrueSound+=audio1.cap()+" ";
Pos=audio2.lastIndexIn(ExVideo);
if (Pos>-1)ExTrueSound+=audio2.cap()+" ";
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.daudio.value='"+ExTrueSound+"'");

//перевод
Descr=wb2->page()->mainFrame()->evaluateJavaScript("qt.jQuery('p:contains(\\'Перевод\\')').text()").toString();
if(Descr.isEmpty())Descr=wb2->page()->mainFrame()->evaluateJavaScript("qt.jQuery('p:contains(\\'перевод\\')').text()").toString();
Descr=Descr.toLower();

for(int it1=0;it1<12;it1++){
  if(Descr.indexOf(transl[it1])>-1){
  ExTranslation=QString::number(it1+1);
  break;
  }
}
if(ExTranslation.isEmpty()&&Descr.indexOf("закадровый")>-1){
    if(Descr.indexOf("одноголосый")>-1)ExTranslation="4";
    if(Descr.indexOf("двухголосый")>-1)ExTranslation="5";
    if(Descr.indexOf("многоголосый")>-1)ExTranslation="6";
}
if(ExTranslation.isEmpty()&&KpCountry.contains(("россия")||("ссср")))ExTranslation="12";
if(ExTranslation.isEmpty())ExTranslation="11";
ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.translation.selectedIndex="+ExTranslation);

//загрузка файла

ExFileDown=wb2->page()->mainFrame()->evaluateJavaScript("qt.jQuery('a[href^=\\'/torrent/\\']').attr('href')").toString();
/*if(!ExFileDown.isEmpty())
 {
 QNetworkAccessManager *FileDown = new QNetworkAccessManager();
 connect(FileDown, SIGNAL(finished(QNetworkReply*)),this, SLOT(FileDownloader(QNetworkReply*)));
 qDebug()<<ExFileDown;
 FileDown->get(QNetworkRequest(QUrl("http://www.ex.ua"+ExFileDown)));
 }*/
// wb4->load(QUrl(Url1));

}
//-------------------------------------------------------------------------------
void MainWindow::pb_click(bool)//скрытие выбора картинок
{
    dial->hide();
}
//-------------------------------------------------------------------------------
void MainWindow::FileDownloader(QNetworkReply *reply)
{
     if (reply->error() == QNetworkReply::NoError)
     {
         if(reply->hasRawHeader("Location"))
         {
             QString locationUrl= reply->header(QNetworkRequest::LocationHeader).toString();
             qDebug()<<locationUrl;
             QNetworkAccessManager *FileDown2 = new QNetworkAccessManager();
             FileDown2->get(QNetworkRequest(QUrl(locationUrl)));
             connect(FileDown2, SIGNAL(finished(QNetworkReply*)),this, SLOT(FileDownloader(QNetworkReply*)));

         }
         else
         {

                QFile FD(FileName.simplified()+".torrent");
                 if (FD.open(QIODevice::WriteOnly))
                 {
                     FD.write(reply->readAll());
                     FD.flush();
                     FD.close();
                 }
  qDebug()<<"скачан файл"<<FileName;
         }
     }
     else
     {
         QMessageBox *nm1=new QMessageBox;
         nm1->setText("Файл торрента не найден");
         nm1->exec();
         nm1->deleteLater();

     }
        reply->deleteLater();
        reply = 0;
}
//-------------------------------------------------------------------------------
void MainWindow::img_put(QString img)//добавление ссылок на картинки
{
if(done2)
{
    ExIm1.clear();
    dial->setWindowTitle(img);
    ExIm1+=img;
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.screen.value+='"+img+"';");
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.entryform.screen.value+='\\n';");
}
}
//-------------------------------------------------------------------------------
void MainWindow::on_pushButton_6_clicked()
{
    ui->webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByClassName('otpravit')[0].click()");
}
//-------------------------------------------------------------------------------
void MainWindow::on_pushButton_5_clicked()
{
    QDesktopServices::openUrl(QUrl(Url1));
}
//-------------------------------------------------------------------------------

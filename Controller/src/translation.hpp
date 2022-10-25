#ifndef TRANSLATION_H
#define TRANSLATION_H
#include <QTranslator>
#include <QCoreApplication>
#include <QtDebug>
class TranslationTest : public QObject
{
 Q_OBJECT
 Q_PROPERTY(QString emptyString READ getEmptyString NOTIFY languageChanged)

 public:
  TranslationTest() {
   translator = new QTranslator(this);
  }

  QString getEmptyString() {
   return "";
  }

 Q_INVOKABLE void selectLanguage(QString language) {
  if(language == QString("zh")) {
    if(!translator->load("t1_zh", QCoreApplication::applicationDirPath())){
        qWarning("Miss Language Package t1_zh.qm!");
    }
    QCoreApplication::instance()->installTranslator(translator);
  }
  if(language == QString("en")) {
    QCoreApplication::instance()->removeTranslator(translator);
  }
  emit languageChanged();
 }

 signals:
  void languageChanged();

 private:
  QTranslator *translator;
};

#endif // TRANSLATION_H

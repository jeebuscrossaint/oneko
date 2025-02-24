#include <QApplication>
#include "oneko.hh"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    Oneko* oneko = new Oneko();
    oneko->show();
    
    return app.exec();
}
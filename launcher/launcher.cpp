#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <QFileDialog>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setGeometry(600, 200, 320, 300);
    window.setWindowTitle("Launcher");

    QLabel *chip8_label = new QLabel("CHIP-8", &window);
    QPixmap *logo_pixmap = new QPixmap("logo.png");

    chip8_label->setPixmap(*logo_pixmap);

    QLabel *browse_for_rom_label = new QLabel("NO ROM SELECTED", &window);
    browse_for_rom_label->setGeometry(0, 170, 320, 20);
    browse_for_rom_label->setAlignment(Qt::AlignCenter);

    QPushButton *select_rom_button = new QPushButton("BROWSE FOR ROM", &window);
    select_rom_button->setGeometry(0, 200, 320, 50);

    QPushButton *launch_button = new QPushButton("LAUNCH", &window);
    launch_button->setGeometry(0, 250, 320, 50);
    launch_button->setEnabled(false);

    QString file_path;

    QObject::connect(select_rom_button, &QPushButton::clicked, [&]()
                     {
        QString path = QFileDialog::getOpenFileName(&window, "Choose File");
        file_path = path;

        std::string path_string = path.toStdString();

        std::string filename;
        int i = path_string.size();
        while(path_string[--i] != '/');
        filename = path_string.substr(i+1,path_string.size()-i);

        QString q_file_name = QString::fromStdString(filename);
        browse_for_rom_label->setText(q_file_name);
        launch_button->setEnabled(true); });

    QObject::connect(launch_button, &QPushButton::clicked, [&]() {
        QString command = "chip-8 ";
        std::string cmd = (command + file_path).toStdString();

	int pid = fork();

	if(pid == 0){
		char* chip8_path = "chip-8";
		execl(chip8_path, chip8_path, file_path.toStdString().c_str(), NULL);
	}

        QCoreApplication::quit();
    });

    window.show();
    return app.exec();
}

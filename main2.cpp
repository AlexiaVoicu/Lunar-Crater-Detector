#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>

using namespace std;

// Constante pentru categorii (raportate la imaginea redusă)
const float R1 = 50.0f;
const float R2 = 100.0f;
const float R3 = 150.0f;
const float R4 = 200.0f;

// --- FUNCTIILE DE DETECTIE ---

int CratereMici(cv::Mat gray, cv::Mat &img, ofstream &tabel, string numeImg)
{
    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.7, 100, 70, 50, 20, 80);
    for (size_t i = 0; i < circles.size(); i++)
    {
        float x = circles[i][0];
        float y = circles[i][1];
        float r = circles[i][2];

        // Desenăm pe imaginea redusă (vizualizare)
        cv::circle(img, cv::Point(cvRound(x), cvRound(y)), (int)r, cv::Scalar(0, 255, 255), 2);

        // Scriem în tabel datele scalate la dimensiunea ORIGINALĂ (* 8)
        // Imagine, Categorie, X_orig, Y_orig, Raza_orig, Diametru_orig
        tabel << numeImg << ",Mic," << (x * 8) << "," << (y * 8) << "," << (r * 8) << "," << (r * 16) << "\n";
    }
    return circles.size();
}

int CratereMijlocii(cv::Mat gray, cv::Mat &img, ofstream &tabel, string numeImg)
{
    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, 120, 65, 60, 80, 150);
    for (size_t i = 0; i < circles.size(); i++)
    {
        float x = circles[i][0];
        float y = circles[i][1];
        float r = circles[i][2];

        cv::circle(img, cv::Point(cvRound(x), cvRound(y)), (int)r, cv::Scalar(0, 255, 0), 2);
        tabel << numeImg << ",Mediu," << (x * 8) << "," << (y * 8) << "," << (r * 8) << "," << (r * 16) << "\n";
    }
    return circles.size();
}

int CratereMari(cv::Mat gray, cv::Mat &img, ofstream &tabel, string numeImg)
{
    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, 200, 70, 60, 150, 200);
    for (size_t i = 0; i < circles.size(); i++)
    {
        float x = circles[i][0];
        float y = circles[i][1];
        float r = circles[i][2];

        cv::circle(img, cv::Point(cvRound(x), cvRound(y)), (int)r, cv::Scalar(255, 0, 255), 2);
        tabel << numeImg << ",Mare," << (x * 8) << "," << (y * 8) << "," << (r * 8) << "," << (r * 16) << "\n";
    }
    return circles.size();
}

int CratereFoarteMari(cv::Mat gray, cv::Mat &img, ofstream &tabel, string numeImg)
{
    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, 400, 90, 68, 200, 500);
    for (size_t i = 0; i < circles.size(); i++)
    {
        float x = circles[i][0];
        float y = circles[i][1];
        float r = circles[i][2];

        cv::circle(img, cv::Point(cvRound(x), cvRound(y)), (int)r, cv::Scalar(0, 0, 255), 2);
        tabel << numeImg << ",Foarte Mare," << (x * 8) << "," << (y * 8) << "," << (r * 8) << "," << (r * 16) << "\n";
    }
    return circles.size();
}

// --- MAIN ---

int main()
{
    // Creștem limita pentru imagini mari
    setenv("OPENCV_IO_MAX_IMAGE_PIXELS", "2147483648", 1);

    cv::String path("imagini_luna/*.TIF");
    vector<cv::String> fn;
    cv::glob(path, fn, false);

    if (fn.empty())
    {
        cout << "Nu am gasit imagini! Verifica daca folderul 'imagini_luna' contine fisiere .TIF" << endl;
        return -1;
    }

    // Header tabel cu noile coloane de scalare
    ofstream tabel("rezultate_cratere.csv");
    tabel << "Imagine,Categorie,Centru_X_Original,Centru_Y_Original,Raza_Originala_Pixeli,Diametru_Original_Pixeli\n";

    cout << "Am gasit " << fn.size() << " imagini. Incep procesarea..." << endl;

    for (size_t i = 0; i < fn.size(); i++)
    {
        cout << "\n[" << i + 1 << "/" << fn.size() << "] " << fn[i] << endl;

        try
        {
            // Citire la 1/8 din dimensiune pentru a economisi RAM
            cv::Mat img = cv::imread(fn[i], cv::IMREAD_REDUCED_COLOR_8);

            if (img.empty())
            {
                cout << "Sari peste: Imaginea nu poate fi incarcata." << endl;
                continue;
            }

            cout << "Dimensiune procesata (1/8): " << img.cols << "x" << img.rows << endl;

            cv::Mat gray;
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
            cv::normalize(gray, gray, 0, 255, cv::NORM_MINMAX);
            cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2);

            // Apelăm funcțiile (datele vor fi înmulțite cu 8 automat în interiorul lor)
            CratereMici(gray, img, tabel, fn[i]);
            CratereMijlocii(gray, img, tabel, fn[i]);
            CratereMari(gray, img, tabel, fn[i]);
            CratereFoarteMari(gray, img, tabel, fn[i]);

            // Salvare vizuală
            string numeSalvare = "detectat_" + to_string(i) + ".jpg";
            cv::imwrite(numeSalvare, img);

            img.release();
            gray.release();
        }
        catch (const cv::Exception &e)
        {
            cerr << "EROARE la imaginea: " << fn[i] << " -> " << e.what() << endl;
            cerr << "Trec la urmatoarea imagine..." << endl;
            continue;
        }
    }

    tabel.close();
    cout << "\nProcesare finalizata! Datele originale au fost salvate in rezultate_cratere.csv" << endl;
    return 0;
}
//Funciona correctamente
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>  // Para std::fixed y std::setprecision
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <numeric> //para std: iota a asignar una secuencia creciente de valores a un rango de elementos en un contenedor.

using namespace std;

struct Color {
    int r;
    int g;
    int b;

    Color(int r, int g, int b) : r(r), g(g), b(b) {}
};

// Clase Punto para representar un punto en 2D
template<typename T>
class Punto {
public:
    T x;
    T y;

    Punto(T x, T y) : x(x), y(y) {}
};

// This function returns a vector with the points read from the "puntos_N_bloques" file
template<typename T>
std::vector<Punto<T>> readPoints(string nameF) {
    //std::ifstream input("puntos_2_bloques.txt");
    std::ifstream input(nameF);
    std::vector<Punto<T>> points;
    T x, y;

    while (input >> x >> y) {
        points.emplace_back(x, y);
    }

    return points;
}

template<typename T>
void imprimirPuntos(const std::vector<Punto<T>>& puntos) {
    cout<<"Imprimiendo "<<puntos.size()<<" puntos"<<endl;
    for (const auto& punto : puntos) {
        std::cout << "(" << std::fixed << std::setprecision(15) << punto.x << ", " << punto.y << ")" << std::endl;
    }
}

// Clase Nodo
template<typename T>
class Nodo {
public:
    Punto<T> centroide;
    std::vector<Punto<T>> puntos;
    Color color; // color del cluster

    Nodo(const Punto<T>& centroide, const Color& color) : centroide(centroide), color(color) {}

    void agregarPunto(const Punto<T>& punto) {
        puntos.push_back(punto);
    }

    void limpiarPuntos() {
        puntos.clear();
    }
};

// Clase KMeans
template<typename T>
class KMeans {
private:
    int k;  // Número de clusters
    std::vector<Nodo<T>> clusters;

public:
    KMeans(int k_) : k(k_) {}


    void ejecutar(const std::vector<Punto<T>>& puntos);
    void imprimirClusters() const;
    void dibujarClusters() const;
private:
    void asignarPuntosAClusters(const std::vector<Punto<T>>& puntos);
    void inicializarClusters(const std::vector<Punto<T>>& puntos);
    T calcularDistancia(const Punto<T>& punto1, const Punto<T>& punto2);
};

template<typename T>
void KMeans<T>::ejecutar(const std::vector<Punto<T>>& puntos) {
        // Inicializar los clusters con centroides aleatorios

        //imprimirPuntos(puntos);

        inicializarClusters(puntos);

        bool cambios = true;
        while (cambios) {
            cambios = false;

            // Asignar cada punto al cluster más cercano
            asignarPuntosAClusters(puntos);

            // Actualizar los centroides de los clusters
            for (auto& cluster : clusters) {
                if (!cluster.puntos.empty()) {
                    T sumX = 0, sumY = 0;
                    for (const auto& punto : cluster.puntos) {
                        sumX += punto.x;
                        sumY += punto.y;
                    }
                    T nuevoCentroideX = sumX / cluster.puntos.size();
                    T nuevoCentroideY = sumY / cluster.puntos.size();
                    //cout<<"Cent: "<<nuevoCentroideX << " , "<<nuevoCentroideY<<endl;
                    cluster.centroide = Punto<T>(nuevoCentroideX, nuevoCentroideY);
                }
            }
        }
}

template<typename T>
void KMeans<T>::imprimirClusters() const {
        for (int i = 0; i < clusters.size(); ++i) {
            std::cout << "Cluster " << i << ": Centroid = (" << std::fixed << std::setprecision(15) << clusters[i].centroide.x << ", " << clusters[i].centroide.y << ")" << std::endl;
            std::cout << "Points: ";
            std::cout<< "# Points: "<< clusters[i].puntos.size()<<endl;
            /*
            for (const auto& punto : clusters[i].puntos) {
                std::cout << "(" << std::fixed << std::setprecision(15) << punto.x << ", " << punto.y << ")" << std::endl;
            }
            */
            std::cout << std::endl;
        }
}

template<typename T>
void KMeans<T>::dibujarClusters() const {
    std::ofstream output("clusters.html");
    output << "<!DOCTYPE html>\n";
    output << "<html>\n";
    output << "<head>\n";
    output << "<meta charset=\"UTF-8\"/>\n";
    output << "<title>Puntos Coloreados con Zoom</title>\n";
    output << "<link rel=\"stylesheet\" href=\"https://unpkg.com/leaflet@1.2.0/dist/leaflet.css\" />\n";
    output << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximumscale=1.0, user-scalable=no\" />\n";
    output << "<script src=\"https://unpkg.com/leaflet@1.2.0/dist/leaflet.js\"></script>\n";
    output << "</head>\n";
    output << "<body>\n";
    output << "<div id=\"map\" style=\"width: 900px; height: 500px;\"></div>\n";
    output << "<script>\n";
    output << "var puntos = [\n";

    for (const auto& cluster : clusters) {
        output << "{ coordenadas: [" << cluster.centroide.x << ", " << cluster.centroide.y << "], color: 'rgb("
               << cluster.color.r << "," << cluster.color.g << "," << cluster.color.b << ")', centroide: true },\n";
        for (const auto& punto : cluster.puntos) {
            output << "{ coordenadas: [" << punto.x << ", " << punto.y << "], color: 'rgb("
                   << cluster.color.r << "," << cluster.color.g << "," << cluster.color.b << ")' },\n";
        }
    }

    output << "];\n";
    output << "var map = L.map('map').setView([0, 0], 1);\n";
    output << "var pointGroup = L.featureGroup().addTo(map);\n";

    output << "for (var i = 0; i < puntos.length; i++) {\n";
    output << "var punto = puntos[i];\n";
    output << "if (punto.centroide) {\n";
    output << "var marker = L.circleMarker(punto.coordenadas, {\n";
    output << "color: punto.color,\n";
    output << "fillColor: punto.color,\n";
    output << "fillOpacity: 1,\n";
    output << "radius: 8\n";
    output << "}).addTo(pointGroup);\n";
    output << "} else {\n";
    output << "var marker = L.circleMarker(punto.coordenadas, {\n";
    output << "color: punto.color,\n";
    output << "fillColor: punto.color,\n";
    output << "fillOpacity: 1,\n";
    output << "radius: 4\n";
    output << "}).addTo(pointGroup);\n";
    output << "}\n";
    output << "}\n";
    output << "map.fitBounds(pointGroup.getBounds());\n";
    output << "L.control.zoom().addTo(map);\n";
    output << "</script>\n";
    output << "</body>\n";
    output << "</html>\n";
    output.close();
}


template<typename T>
void KMeans<T>::asignarPuntosAClusters(const std::vector<Punto<T>>& puntos) {
    for (auto& cluster : clusters) {
        cluster.limpiarPuntos(); // Limpiar los puntos en el cluster antes de asignar nuevos puntos
    }

    for (const auto& punto : puntos) {
        int clusterActual = 0;
        T distanciaMinima = calcularDistancia(punto, clusters[0].centroide);

        for (int i = 1; i < clusters.size(); ++i) {
            T distancia = calcularDistancia(punto, clusters[i].centroide);
            if (distancia < distanciaMinima) {
                distanciaMinima = distancia;
                clusterActual = i;
            }
        }

        clusters[clusterActual].agregarPunto(punto); // Asignar el punto al cluster más cercano
    }
}


template<typename T>
void KMeans<T>::inicializarClusters(const std::vector<Punto<T>>& puntos) {
    //cout<<"PS: "<<puntos.size()<<endl;
    std::vector<int> indices(puntos.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::random_shuffle(indices.begin(), indices.end());

    for (int i = 0; i < k; ++i) {
        int r = rand() % 256; // Componente R aleatorio
        int g = rand() % 256; // Componente G aleatorio
        int b = rand() % 256; // Componente B aleatorio

        clusters.emplace_back(Nodo<T>(puntos[indices[i]], Color(r, g, b)));
    }
}


template<typename T>
T KMeans<T>::calcularDistancia(const Punto<T>& punto1, const Punto<T>& punto2) {
        T dx = punto1.x - punto2.x;
        T dy = punto1.y - punto2.y;
        return std::sqrt(dx * dx + dy * dy);
}

void dibujarPresentacion() {
    // Títulos
    std::cout << "===================================" << std::endl;
    std::cout << "   Trabajo de K-means" << std::endl;
    std::cout << "  Sequential Version" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << std::endl;

    // Animación de carga
    std::cout << "Loading";
    for (int i = 0; i < 3; ++i) {
        std::cout << ".";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << std::endl;
    std::cout << std::endl;

    // Presentación
    std::cout << "Welcome to the work of K-means" << std::endl;
    std::cout << "In this sequential version of the algorithm" << std::endl;
    std::cout << "the grouping of points in clusters will be carried out" << std::endl;
    std::cout << "based on the distance between the points" << std::endl;
    std::cout << "and the centroids of the clusters." << std::endl;
    std::cout << std::endl;

    std::cout << "Let's start!" << std::endl;
    std::cout << std::endl;
     // Esperar 3 segundos
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Limpiar la consola
    #ifdef _WIN32
        system("cls"); // Para Windows
    #else
        system("clear"); // Para sistemas Unix-like (Linux, macOS, etc.)
    #endif

    // Mostrar solo los títulos
    std::cout << "====================================" << std::endl;
    std::cout << "K-means Sequential Version" << std::endl;
    std::cout << "====================================" << std::endl;
}

template<typename T>
void Test(const std::vector<Punto<T>>& puntos, int k) {

    // Iniciar temporizador
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Crear una instancia de KMeans
    KMeans<T> kmeans(k);

    // Ejecutar el algoritmo
    kmeans.ejecutar(puntos);

    // Detener temporizador y calcular el tiempo transcurrido
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    double tiempo = std::chrono::duration_cast<std::chrono::duration<double>>(end - begin).count();

    // Imprimir el tiempo transcurrido
    std::cout << "Tiempo de ejecución: " << std::fixed << std::setprecision(6) << tiempo << " segundos\n";

    kmeans.dibujarClusters(); // Genera el archivo HTML con los clusters y sus colores asociados
    kmeans.imprimirClusters();
}

int main() {

    dibujarPresentacion();
    string nameFile = "puntos_5_bloques.txt";//"puntos_2_bloques.txt"    "ejemplo.txt"
    std::vector<Punto<float>> points = readPoints<float>(nameFile);

    Test(points, 2);


    return 0;
}



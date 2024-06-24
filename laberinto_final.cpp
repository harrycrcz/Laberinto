#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <utility> // Para std::pair

// Función para crear el laberinto
std::vector<std::vector<char>> crear_laberinto(int filas, int columnas)
{
    // Inicializar el laberinto lleno de paredes ('#')
    std::vector<std::vector<char>> laberinto(filas, std::vector<char>(columnas, '#'));

    // Función para generar un punto de inicio o fin en una pared específica
    auto generar_punto = [&](const std::string &pared)
    {
        if (pared == "arriba")
        {
            return std::make_pair(0, rand() % (columnas - 2) + 1);
        }
        else if (pared == "abajo")
        {
            return std::make_pair(filas - 1, rand() % (columnas - 2) + 1);
        }
        else if (pared == "izquierda")
        {
            return std::make_pair(rand() % (filas - 2) + 1, 0);
        }
        else if (pared == "derecha")
        {
            return std::make_pair(rand() % (filas - 2) + 1, columnas - 1);
        }
        return std::make_pair(-1, -1); // Solo por si acaso
    };

    // Elegir paredes opuestas para inicio y fin
    std::vector<std::string> paredes = {"arriba", "abajo", "izquierda", "derecha"};
    std::string pared_inicio = paredes[rand() % paredes.size()];
    std::string pared_fin;
    if (pared_inicio == "arriba")
        pared_fin = "abajo";
    else if (pared_inicio == "abajo")
        pared_fin = "arriba";
    else if (pared_inicio == "izquierda")
        pared_fin = "derecha";
    else if (pared_inicio == "derecha")
        pared_fin = "izquierda";

    auto inicio = generar_punto(pared_inicio);
    auto fin = generar_punto(pared_fin);

    // Función para realizar DFS y crear caminos
    auto dfs = [&](int x, int y, auto &dfs_ref) -> void
    {
        std::vector<std::pair<int, int>> direcciones = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

        // Generador de números aleatorios
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(direcciones.begin(), direcciones.end(), g);

        for (auto [dx, dy] : direcciones)
        {
            int nx = x + dx, ny = y + dy;

            // Verificar si la nueva posición está dentro del laberinto
            if (nx >= 0 && nx < filas && ny >= 0 && ny < columnas)
            {
                // Verificar si el nuevo camino es válido
                if (laberinto[nx][ny] == '#')
                {
                    // Contar el número de paredes adyacentes
                    int paredes_adyacentes = 0;
                    for (auto [ddx, ddy] : direcciones)
                    {
                        int nnx = nx + ddx, nny = ny + ddy;
                        if (nnx >= 0 && nnx < filas && nny >= 0 && nny < columnas)
                        {
                            if (laberinto[nnx][nny] == '#')
                            {
                                paredes_adyacentes++;
                            }
                        }
                    }

                    // Si hay al menos 3 paredes adyacentes, crear un camino
                    if (paredes_adyacentes >= 3)
                    {
                        laberinto[nx][ny] = ' '; // Crear un camino
                        dfs_ref(nx, ny, dfs_ref);
                    }
                }
            }
        }
    };

    // Establecer los puntos de inicio y fin
    laberinto[inicio.first][inicio.second] = 'S';
    laberinto[fin.first][fin.second] = 'E';

    // Comenzar DFS desde el punto de inicio
    dfs(inicio.first, inicio.second, dfs);

    // Asegurar que haya un camino directo desde el inicio hasta el fin
    auto conectar_inicio_fin = [&](std::pair<int, int> inicio, std::pair<int, int> fin)
    {
        int x1 = inicio.first, y1 = inicio.second;
        int x2 = fin.first, y2 = fin.second;
        while (x1 != x2 || y1 != y2)
        {
            if (x1 < x2)
                x1++;
            else if (x1 > x2)
                x1--;
            if (y1 < y2)
                y1++;
            else if (y1 > y2)
                y1--;
            if (laberinto[x1][y1] == '#')
            {
                laberinto[x1][y1] = ' ';
            }
        }
    };

    conectar_inicio_fin(inicio, fin);

    return laberinto;
}

// Función para encontrar el camino desde el inicio hasta el fin usando DFS
bool encontrar_camino(std::vector<std::vector<char>> &laberinto, int x, int y, int fin_x, int fin_y)
{
    if (x == fin_x && y == fin_y)
    {
        return true;
    }

    // Direcciones para moverse en el laberinto, junto con los caracteres de flechas correspondientes
    std::vector<std::pair<int, int>> direcciones = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    std::vector<char> direcciones_char = {'>', 'v', '<', '^'};

    for (size_t i = 0; i < direcciones.size(); ++i)
    {
        int nx = x + direcciones[i].first, ny = y + direcciones[i].second;

        // Verificar si la nueva posición está dentro del laberinto y es un camino
        if (nx >= 0 && nx < laberinto.size() && ny >= 0 && ny < laberinto[0].size() && (laberinto[nx][ny] == ' ' || laberinto[nx][ny] == 'E'))
        {
            if (laberinto[nx][ny] != 'E')
            {
                laberinto[nx][ny] = direcciones_char[i]; // Marcar la dirección del camino
            }

            if (encontrar_camino(laberinto, nx, ny, fin_x, fin_y))
            {
                return true;
            }

            if (laberinto[nx][ny] != 'E')
            {
                laberinto[nx][ny] = ' '; // Desmarcar si no se encuentra un camino
            }
        }
    }

    return false;
}

// Función para imprimir el laberinto
void imprimir_laberinto(const std::vector<std::vector<char>> &laberinto)
{
    for (const auto &fila : laberinto)
    {
        for (const auto &celda : fila)
        {
            std::cout << celda << ' ';
        }
        std::cout << '\n';
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    // Pedir al usuario el tamaño del laberinto
    int filas, columnas;
    std::cout << "Ingrese el número de filas del laberinto: ";
    std::cin >> filas;
    std::cout << "Ingrese el número de columnas del laberinto: ";
    std::cin >> columnas;

    // Crear y mostrar el laberinto
    auto laberinto = crear_laberinto(filas, columnas);
    imprimir_laberinto(laberinto);

    // Encontrar y marcar el camino desde el inicio hasta el fin
    std::pair<int, int> inicio, fin;
    for (int i = 0; i < filas; ++i)
    {
        for (int j = 0; j < columnas; ++j)
        {
            if (laberinto[i][j] == 'S')
            {
                inicio = {i, j};
            }
            else if (laberinto[i][j] == 'E')
            {
                fin = {i, j};
            }
        }
    }

    encontrar_camino(laberinto, inicio.first, inicio.second, fin.first, fin.second);

    std::cout << "\nLaberinto con camino encontrado:\n";
    imprimir_laberinto(laberinto);

    // Esperar a que el usuario presione Enter para salir
    std::cout << "Presiona Enter para salir...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}

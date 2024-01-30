/**
 * @file main.cpp
 * @brief Implementacja programu do planowania tras przy użyciu algorytmu Dijkstry.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <limits>
#include <vector>

typedef std::map<std::string, std::set<std::pair<std::string, int>>> Graph;

/**
 * @brief Ładuje graf z pliku.
 * @param fileName Nazwa pliku zawierającego połączenia drogowe.
 * @return Graph Wczytany graf.
 */

Graph loadFromFile(const std::string& fileName) {
    Graph graph;
    std::ifstream graphFile(fileName);
    if (graphFile) {
        std::string line;
        while (std::getline(graphFile, line)) {
            std::stringstream ss(line);
            std::string source, destination;
            int distance;
            if (!(ss >> source)) {
                continue;
            }
            if (!(ss >> destination)) {
                continue;
            }
            if (!(ss >> distance)) {
                continue;
            }
            graph[source].insert({ destination, distance });
            graph[destination];
        }
        graphFile.close();
    }
    return graph;
}

/**
 * @brief Wyświetla graf na konsoli.
 * @param graph Graf do wyświetlenia.
 */

void displayGraph(const Graph& graph) {
    for (const auto& node : graph) {
        std::cout << "Wezel: " << node.first << std::endl;
        int counter = 1;
        for (const auto& neighbor : node.second) {
            std::cout << "Polaczenie " << counter << ": " << neighbor.first << " (Odleglosc: " << neighbor.second << ")" << std::endl;
            counter++;
        }
        std::cout << std::endl;
    }
}


/**
 * @brief Zapisuje wiadomość z wynikami do pliku.
 * @param resultFileName Nazwa pliku do zapisu wyniku.
 * @param message Wiadomość z wynikami do zapisania.
 */

void saveResultsToFile(const std::string& resultFileName, const std::string& message) {
    std::ofstream resultFile(resultFileName, std::ios::app);

    if (resultFile.is_open()) {
        resultFile << message;
        resultFile.close();
    }
    else {
        std::cerr << "Nie można otworzyć pliku " << resultFileName << " do zapisu wyników." << std::endl;
    }
}

/**
 * @brief Wykonuje algorytm Dijkstry w celu znalezienia najkrótszych ścieżek.
 * @param graph Graf, w którym mają być szukane ścieżki.
 * @param start Początkowy węzeł.
 * @param end Węzeł docelowy.
 * @return std::map<std::string, std::pair<int, std::string>> Mapa węzłów z ich odległościami i poprzednimi węzłami.
 */

std::map<std::string, std::pair<int, std::string>> Dijkstra(const Graph& graph, const std::string& start, const std::string& end) {
    std::map<std::string, std::pair<int, std::string>> paths;

    for (const auto& node : graph) {
        if (node.first == start) {
            paths[node.first] = std::make_pair(0, "");
        }
        else {
            paths[node.first] = std::make_pair(std::numeric_limits<int>::max(), "");
        }
    }

    std::set<std::pair<int, std::string>> queue;
    queue.insert(std::make_pair(0, start));

    while (!queue.empty()) {
        std::string current = queue.begin()->second;
        queue.erase(queue.begin());

        if (graph.count(current) > 0) {
            for (const auto& neighbor : graph.at(current)) {
                int newDistance = paths[current].first + neighbor.second;

                if (newDistance < paths[neighbor.first].first) {
                    paths[neighbor.first].first = newDistance;
                    paths[neighbor.first].second = current;
                    queue.insert(std::make_pair(newDistance, neighbor.first));
                }
            }
        }
        else {
            break;
        }
    }

    return paths;
}

/**
 * @brief Tworzy wiadomość z wynikami trasy.
 * @param start Początkowy węzeł.
 * @param end Węzeł docelowy.
 * @param totalDistance Całkowita odległość trasy.
 * @param graph Graf.
 * @param paths Najkrótsze ścieżki.
 * @return std::string Wiadomość z wynikami.
 */

std::string createResultsMessage(const std::string& start, const std::string& end, int totalDistance, const Graph& graph, const std::map<std::string, std::pair<int, std::string>>& paths) {
    std::ostringstream resultMessage;
    resultMessage << "Trasa: " << start << " --> " << end << " (" << totalDistance << " km):" << std::endl;

    std::vector<std::string> path;
    std::string current = end;

    while (!current.empty()) {
        path.push_back(current);
        current = paths.at(current).second;
    }

    std::reverse(path.begin(), path.end());

    for (size_t i = 0; i < path.size() - 1; ++i) {
        std::string currentCity = path[i];
        std::string nextCity = path[i + 1];
        int distance = 0;

        for (const auto& neighbor : graph.at(currentCity)) {
            if (neighbor.first == nextCity) {
                distance = neighbor.second;
                break;
            }
        }
        resultMessage << currentCity << " --> " << nextCity << " " << distance << " km" << std::endl;
    }

    resultMessage << std::endl;

    return resultMessage.str();
}

/**
 * @brief Główna funkcja, która czyta pliki wejściowe i wykonuje planowanie trasy.
 * @return int Status wyjścia.
 */

int main() {
    /*auto graph = loadFromFile("roads.txt");
    displayGraph(graph);

    std::ofstream resultFile("result.txt");
    std::ifstream routesFile("routes.txt");*/

    std::string roadsFileName, routesFileName, resultFileName;

    std::cout << "Podaj nazwe pliku z polaczeniami drogowymi (graf): ";
    std::cin >> roadsFileName;

    std::ifstream roadsFile(roadsFileName);
    if (!roadsFile.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku " << roadsFileName << std::endl;
        return 1;
    }

    std::cout << "Podaj nazwe pliku z trasami do wyznaczenia: ";
    std::cin >> routesFileName;

    std::ifstream routesFile(routesFileName);
    if (!routesFile.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku " << routesFileName << std::endl;
        return 1;
    }

    std::cout << "Podaj nazwe pliku wyjsciowego: ";
    std::cin >> resultFileName;

    std::ofstream resultFile(resultFileName);
    if (!resultFile.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku " << resultFileName << " do zapisu rezultatu." << std::endl;
        return 1;
    }

    auto graph = loadFromFile("roads.txt");
    displayGraph(graph);

    if (routesFile) {
        std::string line;
        while (std::getline(routesFile, line)) {
            std::stringstream ss(line);
            std::string start, end;
            if (!(ss >> start)) {
                continue;
            }
            if (!(ss >> end)) {
                continue;
            }

            auto paths = Dijkstra(graph, start, end);

            if (graph.count(start) == 0 || graph.count(end) == 0) {
                saveResultsToFile("result.txt", "Trasa: " + start + " --> " + end + " (Brak informacji o polaczeniu)\n\n");
            }
            else  if (paths[end].first != std::numeric_limits<int>::max() && paths.count(end) > 0) {
                std::string resultMessage = createResultsMessage(start, end, paths[end].first, graph, paths);
                saveResultsToFile("result.txt", resultMessage);
            }
            else {
                saveResultsToFile("result.txt", "Trasa: " + start + " --> " + end + " (Trasa niemozliwa do wyznaczenia)\n\n");
            }
        }
        routesFile.close();
        resultFile.close();
    }
}

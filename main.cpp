#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include "connect6.h"
#include "MCTS.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

void drawGame(sf::RenderWindow& window, const State& state, const sf::RectangleShape& boardBackground,
    const sf::VertexArray& verticalLines, const sf::VertexArray& horizontalLines,
    const std::vector<sf::Text>& letters, const std::vector<sf::Text>& numbers,
    sf::CircleShape& playerCircle,  sf::CircleShape& aiCircle,
    const sf::Text& title, int offsetX, int offsetY, float spacing, int playerToken, int aiToken) {
    
    window.clear(sf::Color(50, 52, 55));
    // Dibuja el fondo del tablero
    window.draw(boardBackground);
    // Dibuja las líneas verticales y horizontales 
    window.draw(verticalLines);
    window.draw(horizontalLines);

    // Dibuja las coordenadas
    for (int i = 0; i < 38; i++) {
        window.draw(letters[i]);
        window.draw(numbers[i]);
    }

    // Dibuja las fichas
    for (int i = 0; i < state.size; i++) {
        for (int j = 0; j < state.size; j++) {
            float radius = playerCircle.getRadius();
            if (state.current[i][j] == playerToken) {
                playerCircle.setPosition(offsetX + i * spacing - radius, offsetY + j * spacing - radius);
                window.draw(playerCircle);
            } 
            if (state.current[i][j] == aiToken) {
                aiCircle.setPosition(offsetX + i * spacing - radius, offsetY + j * spacing - radius);
                window.draw(aiCircle);
            }
        }
    }    
    window.draw(title);
    window.display();
}

int main(int argc, char* argv[]) {
    // Inicializar la semilla para números aleatorios
    srand(static_cast<unsigned int>(time(0)));

    // Procesar argumentos de la línea de comandos
    string fichas = "";
    int tpj = 0;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg.find("-fichas=") == 0) {
            fichas = arg.substr(8);
            if (fichas != "blancas" && fichas != "negras") {
                cerr << "Error: Valor inválido para -fichas. Debe ser 'blancas' o 'negras'." << endl;
                return -1;
            }
        } else if (arg.find("-tpj=") == 0) {
            string tpjVal = arg.substr(5);
            tpj = atoi(tpjVal.c_str());
            if (tpj <= 0) {
                cerr << "Error: El tiempo por jugada debe ser un entero positivo" << endl;
                return -1;
            }
        } else {
            cerr << "Error: Argumento desconocido: " << arg << endl;
            return -1;
        }
    }

    // Setear el token correspondiente a la IA y el jugador
    int aiToken = (fichas == "blancas") ? 2 : 1;
    int playerToken = 3 - aiToken;

    // Inicializar el estado del juego
    State state;

    // Dimensiones de la ventana
    const int windowWidth = 900;
    const int windowHeight = 650;

    // Parámetros del tablero
    const int boardLines = 19;
    const int margin = 60;

    // Calcular el espacio disponible para el tablero (área cuadrada)
    int availableWidth = windowWidth - 2 * margin;
    int availableHeight = windowHeight - 2 * margin;
    int boardSize = min(availableWidth, availableHeight);

    // Espaciado entre líneas: hay (boardLines - 1) espacios entre las 19 líneas
    float spacing = static_cast<float>(boardSize) / (boardLines - 1);

    // Calcular offset para centrar el tablero en la ventana
    int offsetX = (windowWidth - boardSize) / 2;
    int offsetY = 80;

    // Crear la ventana
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Connect6");
    sf::Image icon;
    if (!icon.loadFromFile("icon.png")) {
        std::cerr << "Error al cargar el icono" << std::endl;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Cargar fuentes
    sf::Font fontTitle;
    if (!fontTitle.loadFromFile("Fake.ttf")) {
        cerr << "Error al cargar la fuente" << endl;
        return -1;
    }

    sf::Font fontText;
    if (!fontText.loadFromFile("Antone.ttf")) {
        cerr << "Error al cargar la fuente" << endl;
        return -1;
    }

    // Configurar el título
    sf::Text title;
    title.setFont(fontTitle);
    title.setString("Connect 6");
    title.setCharacterSize(45);
    title.setFillColor(sf::Color(232, 95, 67));
    sf::FloatRect titleBounds = title.getLocalBounds();
    float titleX = (window.getSize().x - titleBounds.width) / 2 - titleBounds.left;
    float titleY = 0;
    title.setPosition(titleX, titleY);

    // Fondo del tablero
    float boardXY = boardSize + 30.0f * 2;
    sf::RectangleShape boardBackground(sf::Vector2f(boardXY, boardXY));
    boardBackground.setPosition(offsetX - 30.0f, offsetY - 30.0f);
    boardBackground.setFillColor(sf::Color(222, 184, 135));

    // Fichas (círculos)
    sf::CircleShape black(8), white(8);
    black.setFillColor(sf::Color::Black);
    white.setFillColor(sf::Color::White);

    // Setear fichas segun el turno
    sf::CircleShape aiCircle = (aiToken == 1) ? black : white;
    sf::CircleShape playerCircle = (playerToken == 1) ? black : white;

    // Líneas del tablero
    sf::VertexArray verticalLines(sf::Lines, boardLines * 2);
    sf::VertexArray horizontalLines(sf::Lines, boardLines * 2);

    // Letras y números para las coordenadas
    vector<sf::Text> letters, numbers;

    for (int i = 0; i < boardLines; ++i) {
        float x = offsetX + i * spacing;
        verticalLines[i * 2].position = sf::Vector2f(x, offsetY);
        verticalLines[i * 2].color = sf::Color::Black;
        verticalLines[i * 2 + 1].position = sf::Vector2f(x, offsetY + boardSize);
        verticalLines[i * 2 + 1].color = sf::Color::Black;

        sf::Text lettersAbove, lettersBelow;
        lettersAbove.setFont(fontText);
        lettersAbove.setString(static_cast<char>('A' + i));
        lettersAbove.setCharacterSize(10);
        lettersAbove.setFillColor(sf::Color(50, 52, 55));
        lettersBelow = lettersAbove;
        lettersAbove.setPosition(x - 3, offsetY - 20);
        lettersBelow.setPosition(x - 3, offsetY + boardSize + 10);

        letters.push_back(lettersAbove);
        letters.push_back(lettersBelow);
    }

    for (int j = 0; j < boardLines; ++j) {
        float y = offsetY + j * spacing;
        horizontalLines[j * 2].position = sf::Vector2f(offsetX, y);
        horizontalLines[j * 2].color = sf::Color::Black;
        horizontalLines[j * 2 + 1].position = sf::Vector2f(offsetX + boardSize, y);
        horizontalLines[j * 2 + 1].color = sf::Color::Black;

        sf::Text numbersAbove, numbersBelow;
        numbersAbove.setFont(fontText);
        numbersAbove.setCharacterSize(10);
        numbersAbove.setFillColor(sf::Color(50, 52, 55));
        numbersAbove.setString(to_string(1 + j));
        numbersBelow = numbersAbove;
        numbersAbove.setPosition(offsetX - 20, y - 5);
        numbersBelow.setPosition(offsetX + boardSize + 10, y - 5);

        numbers.push_back(numbersAbove);
        numbers.push_back(numbersBelow);
    }

    int movesLeft = 1;

    // Bucle principal del juego
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                float relativeX = mousePosition.x - offsetX;
                float relativeY = mousePosition.y - offsetY;

                if (relativeX >= 0 && relativeX <= boardSize && relativeY >= 0 && relativeY <= boardSize) {
                    int col = round(relativeX / spacing);
                    int row = round(relativeY / spacing);

                    // Turno del humano
                    if (state.currentPlayer == playerToken && state.makeMove(col, row, playerToken)){
                        movesLeft-- ;
                        
                        drawGame(window, state, boardBackground, verticalLines, horizontalLines,
                            letters, numbers, playerCircle, aiCircle, title,
                            offsetX, offsetY, spacing, playerToken, aiToken);

                        if (state.checkWin(playerToken)) {
                            std::cout << "¡Has ganado!" << std::endl;
                            window.close();
                        }
                        if(movesLeft == 0){
                            state.currentPlayer = aiToken;
                            movesLeft = 2;
                        }
                    }

                    // Turno del agente
                    if (state.currentPlayer == aiToken) {
                        MCTS mcts(state, aiToken, tpj); 
                        auto move = mcts.findBestMove();
                        if (move.first != -1 && move.second != -1) {
                            state.makeMove(move.first, move.second, aiToken);
                            movesLeft-- ;

                            drawGame(window, state, boardBackground, verticalLines, horizontalLines,
                                letters, numbers, playerCircle, aiCircle, title,
                                offsetX, offsetY, spacing, playerToken, aiToken);
                            
                            if (state.checkWin(aiToken)) {
                                std::cout << "¡El agente ha ganado!" << std::endl;
                                window.close();
                            }
                            if(movesLeft == 0){
                                state.currentPlayer = playerToken;
                                movesLeft = 2;
                            }
                        }
                    }
                }
            }
        }

        drawGame(window, state, boardBackground, verticalLines, horizontalLines,
            letters, numbers, playerCircle, aiCircle, title,
            offsetX, offsetY, spacing, playerToken, aiToken);
    }

    return 0;
}
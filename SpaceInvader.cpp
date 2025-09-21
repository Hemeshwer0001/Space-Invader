
#include <iostream>
#include <random> // to provide random coordinates to aliens
#include <fstream> // to store data in text files
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace std;

void moveShip(sf::Sprite& ship, float moveX, float deltaTime, bool Over){
    // here we will make click listener for the spaceship
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && Over == false){
        ship.move(-moveX*deltaTime, 0);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && Over == false){
        ship.move(moveX*deltaTime, 0);
    }

    if(ship.getPosition().x+64 >= 800){
        ship.setPosition(736, 536);
    }
    else if(ship.getPosition().x <= 0){
        ship.setPosition(0, 536);
    }
}

void loadAliens(vector<sf::Sprite>&aliens, sf::Sprite& alien, int& totalAliens){
    // we will use <random> header file to get random coordinates for the aliens
    random_device rd;
    mt19937 gen(rd());
    
    for(int i = 0; i<totalAliens; i++){
        uniform_int_distribution<> X(0, 730); // range [0, 736]
        uniform_int_distribution<> Y(0, 200); // range [0, 200]
        int AlienX = X(gen); // will help us get random values
        int AlienY = Y(gen);
        alien.setPosition(AlienX, AlienY);
        aliens.push_back(alien);
    }
}

void displayAliens(vector<sf::Sprite>& aliens, sf::RenderWindow& window){
    for(int i = 0; i<aliens.size(); i++){
        window.draw(aliens[i]);
    }
}

void moveAliens(vector<sf::Sprite>& aliens, vector<sf::Vector2f>& AlienMovement, float deltaTime, bool& over){
    // moving aliens
    for(int i = 0; i<aliens.size(); i++){
        aliens[i].move(AlienMovement[i]*deltaTime);
    }

    // keeping the alien in bounds
    for(int i = 0; i<aliens.size(); i++){
        if(aliens[i].getPosition().x+64 >= 800){
            aliens[i].setPosition(736, aliens[i].getPosition().y+40);
            AlienMovement[i].x *= -1;
        }
        else if(aliens[i].getPosition().x <= 0){
            aliens[i].setPosition(0, aliens[i].getPosition().y+40);
            AlienMovement[i].x *= -1;
        }
        else if(aliens[i].getPosition().y+64 >= 560){
            over = true;
            for(int i = 0; i<aliens.size(); i++){
                AlienMovement[i].x = 0;
            }
            break;
        }
    }
}

bool AlienMissileCollision(sf::Sprite& missile, vector<sf::Sprite>& aliens, string& state, sf::Sound& missileCollision){ // it wil tell us if the missile has collided with any alien
    for(int i = 0; i<aliens.size(); i++){
        if(missile.getGlobalBounds().intersects(aliens[i].getGlobalBounds())){
            missileCollision.play();
            missile.setPosition(-100, -100); // taking misslie off window to avoid hitting to another alien because of high framerate
            state = "rest";
            aliens.erase(aliens.begin()+i); // it will remove the exact alien that we hit with our missile
            return true; // return true to make it known that we hit an alien
        }
    }
    return false; // if the loop termiates normally.. no collision
}

void gameOver(sf::Text& GameOver){
    GameOver.setCharacterSize(60);
    GameOver.setFillColor(sf::Color::Magenta);
    GameOver.Bold;
    GameOver.setPosition(230, 300);
    GameOver.setString("Game Over!");
}


int main(){
    // I am connecting this to a text file...
    fstream fh, fhwrite;
    fh.open("HighScore.txt", ios::in); // opening the file in both read and write mode
    string highScore = "";
    char ch;
    while(fh.get(ch)){
        highScore += ch;
    }
    int highSco = 0;
    if(highScore != "") highSco = stoi(highScore); // storing in int format for comparision


    // creating a window 
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Space Invader");
    sf::Clock clock;
    bool Over = false;

    // creating sound for Missile Launch
    sf::SoundBuffer missileBuffer;
    missileBuffer.loadFromFile("missile.wav");
    sf::Sound missileLaunch;
    missileLaunch.setBuffer(missileBuffer);

    // Creating sound for Missile and Alien Collision
    sf::SoundBuffer collisionBuffer;
    collisionBuffer.loadFromFile("Explosion.wav");
    sf::Sound MissileCollision;
    MissileCollision.setBuffer(collisionBuffer);

    // creating a font
    sf::Font font;
    font.loadFromFile("FreeRoyalty.ttf");
    sf::Text GameOver;
    GameOver.setFont(font);

    // score 
    int score = 0;
    sf::Text Score;
    Score.setFont(font);
    Score.setFillColor(sf::Color::Cyan);
    Score.setCharacterSize(30);
    Score.setPosition(10, 10);

    // highScore
    sf::Text HighScore;
    HighScore.setFont(font);
    HighScore.setCharacterSize(30);
    HighScore.setPosition(570, 0);
    HighScore.setFillColor(sf::Color::Green);
    HighScore.Bold;

    // Loading BackGround
    sf::Texture bgTexture;
    bgTexture.loadFromFile("Background.png");
    sf::Sprite backGround;
    backGround.setTexture(bgTexture);

    // creating a spaceShip
    sf::Texture shipTexture;
    shipTexture.loadFromFile("SpaceShip.png");
    sf::Sprite spaceShip;
    spaceShip.setTexture(shipTexture);
    spaceShip.setTextureRect(sf::IntRect(64, 0, 64, 64));
    spaceShip.setPosition(window.getSize().x/2-32, 536);
    float moveShipX = 550.0, moveShipY = 0; 

    // creating aliens
    sf::Texture alienTexture;
    alienTexture.loadFromFile("plunderReaper.png");
    sf::Sprite alien;
    alien.setTexture(alienTexture);
    vector<sf::Sprite> aliens;
    int totalAliens = 6;
    vector<sf::Vector2f> AlienMovement;

    // time to throw bombs (missile)
    sf::Texture missileTexture;
    missileTexture.loadFromFile("missile00.png");
    sf::Sprite missile;
    missile.setTexture(missileTexture);
    string state = "rest"; // initially the missile is at rest
    float missileY_change = -600.0;
    

    for(int i = 0; i<totalAliens; i++){ // setting movements for all aliens
        AlienMovement.push_back({700.0, 0});
    }
    loadAliens(aliens, alien, totalAliens); // loading aliens

    // gameloop
    while(window.isOpen()){
        float deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && state == "rest" && Over == false){ // the missile can be launched only when it is at rest
            state = "fire"; // missile is launched
            missileLaunch.play();
        }

        moveShip(spaceShip, moveShipX, deltaTime, Over); // it will handle ship movements and bounds
        moveAliens(aliens, AlienMovement, deltaTime, Over);

        if(state == "rest") missile.setPosition(spaceShip.getPosition().x+20, spaceShip.getPosition().y+20);
        else{ // the missile is launched
            missile.move(0, missileY_change*deltaTime);
            if(missile.getPosition().y <= 0){
                state = "rest";
            }
            else if(AlienMissileCollision(missile, aliens, state, MissileCollision)){
                // our missile did hit an alien.
                int needAlien = 1; // we removed one alien.. so, we will bring it back
                loadAliens(aliens, alien, needAlien); // it will bring back the ailens that we killed
                score++;
            }
        }
        if(Over == true){
            state = "rest"; // forcefully bring the missile at rest
            gameOver(GameOver);
        }

        if(score > highSco) highSco = score;

        Score.setString("Score : "+to_string(score));
        HighScore.setString("High Score : "+to_string(highSco));

        window.clear(sf::Color::Black);
        window.draw(backGround);
        window.draw(missile);
        window.draw(spaceShip);
        displayAliens(aliens, window);
        window.draw(GameOver);
        window.draw(Score);
        window.draw(HighScore);
        window.display();
    }
    fh.close();
    // after the end of program we will check if we got a new high score or not
    if(highScore == "" || (score > stoi(highScore))){
        fhwrite.open("HighScore.txt", ios::out);
        fhwrite<<score;
        fhwrite.close();
    }
}


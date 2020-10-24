#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <ctime>
#include <fstream>

#define pi 3.1415

using namespace std;

int map1[15][20];

float playerSpeed = 2, bulletSpeed = 6;
int hp = 10000, ammo = 30;

int rotation(int mouse_x, int mouse_y, int pl_x, int pl_y) {
    return atan2((mouse_y - pl_y), (mouse_x - pl_x)) / pi * 180;
}

int range(int x1, int y1, int x2, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

struct bulletO{
    float x, y, angle;
    int created, which;
};

struct enemyStr {
    float x, y, angle;
    int can_shoot_bot1, timer, last_time, cooldownShoot, hp;
};

int main() {
    ifstream fin("map.m");
    for (int o = 0; o < 15; o++) {
        for (int e = 0; e < 20; e++) {
            fin >> map1[o][e];
            if (map1[o][e] == 1) {
                cout << "Block\n";
            }
        }
    }
    fin.close();
    float pl_x = 0, pl_y = 0;
    clock_t flagTime = clock();
    int can_shoot = 1, timer = 0, last_time = -1, cooldownShoot = 50;
    vector <bulletO> bullets;
    sf::RenderWindow window(sf::VideoMode(640, 480), "TDS");
    sf::Texture texture;
    if (!texture.loadFromFile("player.png", sf::IntRect(0, 0, 45, 30))) {
        cout << "5";
    }
    sf::Texture enemyT;
    if (!enemyT.loadFromFile("enemy.png")) {
        cout << "5";
    }
    sf::Texture bulletT;
    if (!bulletT.loadFromFile("bullet.png", sf::IntRect(0, 0, 60, 25))) {
        cout << "5";
    }
    sf::Texture bulletsBuffT;
    if (!bulletsBuffT.loadFromFile("bullets+.png")) {
        cout << "5";
    }
    sf::Texture hpBuffT;
    if (!hpBuffT.loadFromFile("hp+.png")) {
        cout << "5";
    }
    sf::Texture backGT;
    if (!backGT.loadFromFile("background1.png")) {
        cout << "5";
    }
    sf::Texture blockT;
    if (!blockT.loadFromFile("wall.png")) {
        cout << "5";
    }

    vector<sf::Sprite> collisionBlocks;
    vector<sf::Sprite> collisionbulletsBuffS;
    vector<sf::Sprite> stayedEnemy;
    vector<enemyStr> enemyInfo;
    vector<pair<int, int> > bulBufInfo;
    for (int o = 0; o < 15; o++) {
        for (int e = 0; e < 20; e++) {
            if (map1[o][e] == 1) {
                sf::Sprite blockS;
                blockS.setTexture(blockT);
                blockS.setPosition(e * 32, o * 32);
                collisionBlocks.push_back(blockS);
                window.draw(blockS);
            }
            if (map1[o][e] == 3) {
                sf::Sprite bulletsBuffS;
                bulletsBuffS.setTexture(bulletsBuffT);
                bulletsBuffS.setOrigin(sf::Vector2f(40.f, 40.f));
                bulletsBuffS.setPosition(e * 32, o * 32);
                collisionbulletsBuffS.push_back(bulletsBuffS);
                //cout << e << " " << o << endl;
                pair<int, int> a (e * 32, o * 32);
                bulBufInfo.push_back(a);
                window.draw(bulletsBuffS);
            }
            if (map1[o][e] == 2) {
                sf::Sprite enemyS;
                enemyS.setTexture(enemyT);
                enemyS.setOrigin(sf::Vector2f(19.f, 13.f));
                enemyS.setPosition(e * 32, o * 32);
                enemyS.setRotation(rotation(e * 32, o * 32, pl_x, pl_y) + 180);
                enemyStr ff;
                ff.x = enemyS.getPosition().x;
                ff.y = enemyS.getPosition().y;
                ff.angle = enemyS.getRotation();
                ff.can_shoot_bot1 = 1;
                ff.last_time = -1;
                ff.timer = 0;
                ff.cooldownShoot = 100;
                ff.hp = 100;
                enemyInfo.push_back(ff);
                stayedEnemy.push_back(enemyS);
                window.draw(enemyS);
            }
        }
    }

    bulletT.setSmooth(true);
    texture.setSmooth(true);
    enemyT.setSmooth(true);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setOrigin(sf::Vector2f(13.f, 13.f));

    sf::Sprite backS;
    backS.setTexture(backGT);

    sf::View view(sf::FloatRect(0.f, 0.f, 640.f, 480.f));
    window.setView(view);

    while (window.isOpen()) {
        clock_t nowTime = clock();
        //cout << nowTime << endl;
        if (nowTime - flagTime > 10) {
            int fps = 1000 / (nowTime - flagTime);
            //cout << fps << endl;
            flagTime = nowTime;
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::Resized)
                {
                    sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }
            }

            sf::Vector2i localPosition = sf::Mouse::getPosition(window);
            sprite.setRotation(rotation(localPosition.x, localPosition.y, pl_x, pl_y));
            
            if ((ammo > 0) && (can_shoot == 1) && (sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
                bulletO bul;
                bul.x = pl_x;
                bul.y = pl_y;
                bul.angle = rotation(localPosition.x, localPosition.y, pl_x, pl_y);
                bul.created = 1;
                bul.which = 1;
                bullets.push_back(bul);
                can_shoot = 0;
                ammo--;
                cout << "6";
                last_time = timer;
            }
            int p = 0;
            for (sf::Sprite sp : stayedEnemy) {
                if ((enemyInfo[p].can_shoot_bot1 == 1) && (range(enemyInfo[p].x, enemyInfo[p].y, pl_x, pl_y) <= 400)) {
                    bulletO bul;
                    bul.x = enemyInfo[p].x;
                    bul.y = enemyInfo[p].y;
                    bul.angle = rotation(pl_x, pl_y, enemyInfo[p].x, enemyInfo[p].y);
                    bul.created = 1;
                    bul.which = 2;
                    bullets.push_back(bul);
                    enemyInfo[p].can_shoot_bot1 = 0;
                    enemyInfo[p].last_time = enemyInfo[p].timer;
                }
                enemyInfo[p].timer++;
                if ((enemyInfo[p].last_time != -1) && (enemyInfo[p].timer - enemyInfo[p].last_time == enemyInfo[p].cooldownShoot)) {
                    enemyInfo[p].can_shoot_bot1 = 1;
                    enemyInfo[p].last_time = -1;
                    enemyInfo[p].timer = 0;
                }
                p++;
            }
            timer++;
            if ((last_time != -1) && (timer - last_time == cooldownShoot)) {
                can_shoot = 1;
                last_time = -1;
                timer = 0;
            }

            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
                view.move(100.f, 0.f);
                cout << "Z\n";
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
                view.rotate(5.f);
                cout << "X\n";
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                sprite.move(sf::Vector2f(-playerSpeed, 0.f));
                pl_x -= playerSpeed;
                for (sf::Sprite i : collisionBlocks)
                    if (sprite.getGlobalBounds().intersects(i.getGlobalBounds())) {
                        sprite.move(sf::Vector2f(playerSpeed, 0.f));
                        pl_x += playerSpeed;
                        
                    }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                sprite.move(sf::Vector2f(playerSpeed, 0.f));
                pl_x += playerSpeed;
                for (sf::Sprite i : collisionBlocks)
                    if (sprite.getGlobalBounds().intersects(i.getGlobalBounds())) {
                        sprite.move(sf::Vector2f(-playerSpeed, 0.f));
                        pl_x -= playerSpeed;
                        
                    }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                sprite.move(sf::Vector2f(0.f, -playerSpeed));
                pl_y -= playerSpeed;
                for (sf::Sprite i : collisionBlocks)
                    if (sprite.getGlobalBounds().intersects(i.getGlobalBounds())) {
                        sprite.move(sf::Vector2f(0.f, playerSpeed));
                        pl_y += playerSpeed;
                        
                    }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                sprite.move(sf::Vector2f(0.f, playerSpeed));
                pl_y += playerSpeed;
                for (sf::Sprite i : collisionBlocks)
                    if (sprite.getGlobalBounds().intersects(i.getGlobalBounds())) {
                        sprite.move(sf::Vector2f(0.f, -playerSpeed));
                        pl_y -= playerSpeed;
                        
                    }
            }
            int y = 0;
            for (bulletO i : bullets) {
                if ((i.x < 0) || (i.x > 640) || (i.y < 0) || (i.y > 480)) {
                    bullets.erase(bullets.begin() + y);
                    cout << "8" << endl;
                }
                
                y++;
            }

            window.clear();
            window.draw(backS);
            for (int o = 0; o < 15; o++) {
                for (int e = 0; e < 20; e++) {
                    if (map1[o][e] == 1) {
                        sf::Sprite blockS;
                        blockS.setTexture(blockT);
                        blockS.setPosition(e * 32, o * 32);
                        window.draw(blockS);
                    }
                    if (map1[o][e] == 2) {
                        sf::Sprite enemyS;
                        enemyS.setTexture(enemyT);
                        enemyS.setOrigin(sf::Vector2f(19.f, 13.f));
                        enemyS.setPosition(e * 32, o * 32);
                        enemyS.setRotation(rotation(e * 32, o * 32, pl_x, pl_y) + 180);
                        window.draw(enemyS);
                    }
                    if (map1[o][e] == 3) {
                        sf::Sprite bulletsBuffS;
                        bulletsBuffS.setTexture(bulletsBuffT);
                        bulletsBuffS.setOrigin(sf::Vector2f(34.f, 34.f));
                        bulletsBuffS.setPosition(e * 32, o * 32);
                        window.draw(bulletsBuffS);
                    }
                }
            }
            window.draw(sprite);
            int dd = 0;
            for (sf::Sprite bu : collisionbulletsBuffS) {
                if (sprite.getGlobalBounds().intersects(bu.getGlobalBounds())) {
                    if (!collisionbulletsBuffS.empty()) {
                        ammo += 20;
                        cout << bulBufInfo[dd].first / 32 << " " << bulBufInfo[dd].second / 32 << endl;
                        map1[bulBufInfo[dd].second / 32][bulBufInfo[dd].first / 32] = 0;
                        collisionbulletsBuffS.erase(collisionbulletsBuffS.begin() + dd);
                    }
                }
                dd++;
            }
            int kk = 0;
            for (bulletO& i : bullets) {
                sf::Sprite bulletSP;
                bulletSP.setTexture(bulletT);
                bulletSP.setOrigin(sf::Vector2f(10.f, 12.f));
                bulletSP.setPosition(i.x, i.y);
                i.x += cos(i.angle / 180 * pi) * bulletSpeed;
                i.y += sin(i.angle / 180 * pi) * bulletSpeed;
                bulletSP.setRotation(i.angle);
                if (bulletSP.getGlobalBounds().intersects(sprite.getGlobalBounds())) {
                    if (i.which == 2) {
                        hp -= 10;
                        if (!bullets.empty()) {
                            bullets.erase(bullets.begin() + kk);
                        }
                        cout << "9\n";
                    }
                }
                int hg = 0;
                for (sf::Sprite inf : stayedEnemy) {
                    /*if (enemyInfo[hg].hp <= 0) {
                        map1[(int)enemyInfo[hg].x][(int)enemyInfo[hg].y] = 0;
                    }*/
                    if (bulletSP.getGlobalBounds().intersects(inf.getGlobalBounds())) {
                        //cout << "\nHP: " << enemyInfo[hg].hp << endl;
                        if (i.which == 1) {
                            if (enemyInfo[hg].hp > 10) {
                                enemyInfo[hg].hp -= 10;
                            }
                            else {
                                //cout << "\nX " << enemyInfo[hg].x << " Y: " << enemyInfo[hg].y << endl;
                                map1[(int)enemyInfo[hg].y / 32][(int)enemyInfo[hg].x / 32] = 0;
                                if (!enemyInfo.empty()) {
                                    enemyInfo.erase(enemyInfo.begin() + hg);
                                }
                            }
                            if (!bullets.empty()) {
                                bullets.erase(bullets.begin() + kk);
                            }
                            cout << "10\n";
                        }
                    }
                }
                for (sf::Sprite bl : collisionBlocks)
                    if (bulletSP.getGlobalBounds().intersects(bl.getGlobalBounds())) {
                        if (!bullets.empty()) {
                            bullets.erase(bullets.begin() + kk);
                        }
                    }
                
                kk++;
                window.draw(bulletSP);
            }
            window.display();
        }
    }
    return 0;
}
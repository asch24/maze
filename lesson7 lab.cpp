#include <iostream>
#include <windows.h> // окрашивание консоли и работа с координатами
#include <conio.h>
using namespace std;

// enumeration - перечисление
enum KeyCodes { ENTER = 13, ESCAPE = 27, LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80, SPACEBAR = 32 };
enum Colors { DARKGREEN = 2, RED = 12, YELLOW = 14, BLUE = 9, GREEN = 10 };
enum Objects { HALL, WALL, COIN, ENEMY, MEDICINE };

// const int ENTER = 13;
// const int ESCAPE = 27;

int main()
{

	int coins = 0;
	int total_coins = 0;
	int health = 100;
	int total_enemies = 0;

	system("title Бомбер!");
	// запуск алгоритма генерации случайных чисел
	srand(time(0));
	rand();

	// таблица аски для поиска символов псевдографики
	//for (int code = 0; code < 256; code++)
	//{
	//	cout << code << " - " << (char)code << "\n";
	//}

	// дескриптор окна консоли (порядковый номер окна в системе)
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	// скрыть стандартный мигающий курсор
	CONSOLE_CURSOR_INFO info;
	info.bVisible = false;
	info.dwSize = 100;
	SetConsoleCursorInfo(h, &info);

	const int WIDTH = 50;
	const int HEIGHT = 15;
	int location[HEIGHT][WIDTH] = {};

	// модель локации
	// 0 - коридоры (пустоты)
	// 1 - стена разрушаемая
	// 2 - монетки
	// 3 - враги
	// 4 - лекарство

	// генерация локации
	for (int y = 0; y < HEIGHT; y++) // перебор строк
	{
		for (int x = 0; x < WIDTH; x++) // перебор столбцов
		{
			// по дефолту пишется случайное число
			location[y][x] = rand() % 5; // 0 1 2 3 4

			// стены по краям
			if (x == 0 || y == 0 || x == WIDTH - 1 || y == HEIGHT - 1)
				location[y][x] = WALL;

			// вход и выход
			if (x == 0 && y == 2 || x == WIDTH - 1 && y == HEIGHT - 3)
				location[y][x] = HALL;

			if (location[y][x] == ENEMY) {
				// определяется вероятность того, останется враг или нет
				// допустим, вероястность остаться на уровне - 10%
				int prob = rand() % 10; // 0-9
				if (prob != 0) // 1 2 3 4 5 6 7 8 9
					location[y][x] = HALL;
				else
					total_enemies++;
			}

			if (location[y][x] == COIN) {
				total_coins++;
			}

			if (location[y][x] == MEDICINE) {
				int prob = rand() % 10; // 0-9
				if (prob != 0)
					location[y][x] = HALL;
			}
		}
	}

	// показ локации
	for (int y = 0; y < HEIGHT; y++) // перебор строк
	{
		for (int x = 0; x < WIDTH; x++)
		{
			// cout << location[y][x];
			switch (location[y][x]) {
			case HALL: // коридор
				cout << " ";
				break;
			case WALL: // стена
				SetConsoleTextAttribute(h, DARKGREEN); // 0-255
				cout << (char)177; //(char)219;
				break;
			case COIN: // монетки
				SetConsoleTextAttribute(h, YELLOW);
				cout << (char)15; // 249
				break;
			case ENEMY: // враги
				SetConsoleTextAttribute(h, RED);
				cout << (char)1;
				break;
			case MEDICINE:  // лекарство
				SetConsoleTextAttribute(h, GREEN);
				cout << (char)3;
				break;
			default:
				cout << location[y][x];
				break;
			}
		}
		cout << "\n";
	}

	// размещение ГГ (главного героя)
	COORD position;
	position.X = 1;
	position.Y = 2;

	// функция установки курсора в любую точку на экране консоли
	SetConsoleCursorPosition(h, position);
	SetConsoleTextAttribute(h, BLUE);
	cout << (char)1;

	COORD prev_position = position;

	// игровой движок (интерактив с пользователем)
	while (true) {
		SetConsoleCursorPosition(h, { WIDTH + 5, 0 });
		SetConsoleTextAttribute(h, YELLOW);
		cout << "Coins: " << coins << "/" << total_coins;

		SetConsoleCursorPosition(h, { WIDTH + 5, 1 });
		SetConsoleTextAttribute(h, GREEN);
		cout << "Health: " << health << "%    ";

		int code = _getch(); // функция приостанавливает работу программы, ждёт реакции пользователя
		if (code == 224)
			code = _getch(); // если первый код был 224, то повторный вызов функции вернёт только один, нормальный код

		COORD new_position = position;

		// стирание ГГ в старой позиции
		SetConsoleCursorPosition(h, position);
		SetConsoleTextAttribute(h, BLUE);
		cout << " ";

		// пользователь может нажать любую кнопку (стрелочки), после чего вернётся код нажатой клавиши
		switch (code) {
		case RIGHT:
            if (location[position.Y][position.X + 1] != WALL)
                new_position.X++;
            break;
        case LEFT:
            if (location[position.Y][position.X - 1] != WALL)
                new_position.X--;
            break;
        case UP:
            if (location[position.Y - 1][position.X] != WALL)
                new_position.Y--;
            break;
        case DOWN:
            if (location[position.Y + 1][position.X] != WALL)
                new_position.Y++;
            break;
        default:
            break;
        }

		if (new_position.X != position.X || new_position.Y != position.Y) {
			SetConsoleCursorPosition(h, position);
			cout << " ";

			prev_position = position;
			position = new_position;
		}

		// показ ГГ в новой позиции
		SetConsoleCursorPosition(h, position);
		SetConsoleTextAttribute(h, BLUE);
		cout << (char)1;

		// взаимодействие ГГ с другими объектами в лабиринте

		// сбор монет
		if (location[position.Y][position.X] == COIN) {
			coins++;
			location[position.Y][position.X] = HALL;
		}
		// победа при сборе всех монет
		if (coins == total_coins) {
			MessageBox(0, L"Монеты собраны!", L"Победа", MB_OK);
			break;
		}

		// взаимодействие ГГ с врагами
		if (location[position.Y][position.X] == ENEMY) {
			int damage = 20 + rand() % 6;
			health -= damage;
			location[position.Y][position.X] = HALL;
			total_enemies--;
			if (health <= 0) {
				MessageBox(0, L"Закончилось здоровье!", L"Поражение", MB_OK);
				break;
			}
		}
		// победа при уничтожении всех врагов
		if (total_enemies == 0) {
			MessageBox(0, L"Враги уничтожены!", L"Победа", MB_OK);
			break;
		}

		// взаимодействие ГГ с лекарством
		if (location[position.Y][position.X] == MEDICINE) {
			if (health < 100) {
				health += 5;
				if (health > 100)
					health = 100;
				location[position.Y][position.X] = HALL;
			}
		}

		// если ГГ ушел с клетки с лекарством и не подобрал его, перерисовываем лекарство
		if (location[prev_position.Y][prev_position.X] == MEDICINE && health == 100) {
			SetConsoleCursorPosition(h, prev_position);
			SetConsoleTextAttribute(h, GREEN);
			cout << (char)3;
		}
		else if (location[prev_position.Y][prev_position.X] == HALL) {
			SetConsoleCursorPosition(h, prev_position);
			cout << " ";
		}

		// победа при выходе из лабиринта
		if (position.X == WIDTH - 1 && position.Y == HEIGHT - 3) {
			MessageBox(0, L"Найден выход!", L"Победа", MB_OK);
			break;
		}
	}
}

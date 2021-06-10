/* Место действия этой игры — «вселенная» — это размеченная на клетки поверхность или плоскость — безграничная, ограниченная, или замкнутая(в пределе — бесконечная плоскость).
* 
Каждая клетка на этой поверхности может находиться в двух состояниях : быть «живой»(заполненной) или быть «мёртвой»(пустой).Клетка имеет восемь соседей, окружающих её.

Распределение живых клеток в начале игры называется первым поколением.Каждое следующее поколение рассчитывается на основе предыдущего по таким правилам :
	в пустой(мёртвой) клетке, рядом с которой ровно три живые клетки, зарождается жизнь;
	если у живой клетки есть две или три живые соседки, то эта клетка продолжает жить;
	в противном случае, если соседей меньше двух или больше трёх, клетка умирает(«от одиночества» или «от перенаселённости»)

Игра прекращается, если
	на поле не останется ни одной «живой» клетки
	конфигурация на очередном шаге в точности(без сдвигов и поворотов) повторит себя же на одном из более ранних шагов(складывается периодическая конфигурация)
	при очередном шаге ни одна из клеток не меняет своего состояния(складывается стабильная конфигурация; предыдущее правило, вырожденное до одного шага назад)
	https://neerc.ifmo.ru/wiki/index.php?title=Модели_клеточных_автоматов
	*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include "Menu.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>

/* информация о клетках в мире хранится в переменной world типа поинт для того, чтобы понимать
жива ли она или нет
*/
#define WIDTH 20 // константы для размера поля в высоту
#define HEIGHT 40// константы для размера поля в ширину
#define WORLDS 10  // количество миров для нахождения стабильной конфигурации

struct Point // клетки, если живы, то - 1, если нет - 0, 1 - "#", 0 " " 
{
	bool isLive = 1;
	int x = 0;
	int y = 0;
};

struct StabConf
{
	Point worlds[WIDTH][HEIGHT];
};

struct Game
{
	// генерация первого поколения 
	void firstGen(Point world[WIDTH][HEIGHT], int cells)
	{
		if (cells == 0)
		{			
			for (size_t i = 0; i < WIDTH; i++)
			{
				for (size_t j = 0; j < HEIGHT; j++)
				{
					/*щий правый операнд типа "int" (или приемлемое преобразование отсутствует)
					*/
					int num = rand() % 10000;
					if (num % 2 == 0)
						world[i][j].isLive = 1;
					else
						world[i][j].isLive = 0;
				}
			}
		}
		else
		{
			int k = 0;
			for (size_t i = 0; i < WIDTH; i++)
			{
				for (size_t j = 0; j < HEIGHT; j++)
				{
					int num = rand() % 10000;
					if (num % 2 == 0)
					{
						if (k < cells)
						{
							world[i][j].isLive = 1;
							k++;
						}
						else
							world[i][j].isLive = 0;
					}
					else
						world[i][j].isLive = 0;
				}
			}
		}
		
	}

	// считать всех живых клеток
	int countLive(Point world[WIDTH][HEIGHT])
	{
		int k = 0;
		for (size_t i = 0; i < WIDTH; i++)
		{
			for (size_t j = 0; j < HEIGHT; j++)
			{
				if (world[i][j].isLive == 1)
				{
					k++;
				}
			}
		}
		return k;
	}

	// считать соседей для каждой клетки 
	int count(Point pos, Point world[WIDTH][HEIGHT])
	{
		// считать соседние клетки по Муру
		Point moor;
		moor.x = pos.x - 1;
		moor.y = pos.y - 1;
		// pos - данная клетка
		int k = 0; // соседи данной клетки
		/*
		if (pos.x == 0 || pos.x == 9 || pos.y == 1 || pos.y == 9)
		{
			for (size_t i = pos.x % WIDTH; i < pos.x + 3 ; i++)
			{
				for (size_t j = pos.y % HEIGHT; j < pos.y + 3 ; j++)
				{
					k++;
				}
			}
		}
		*/
		
		if (moor.x || moor.y < 0)
		{		
			moor.x = moor.x % WIDTH;
			moor.y = moor.y % HEIGHT;
			for (size_t i = moor.x; i < moor.x + 3; i++)
			{
				for (size_t j = moor.y; j < moor.y + 3; j++)
				{
					if (world[i][j].isLive == 1)
					{
						/*if (moor.x < 0)*/
						moor.x = moor.x % WIDTH;
						/*if (moor.y < 0)*/
						moor.y = moor.y % HEIGHT;
						k++;
					}
				}
			}
		}
		else
		{
			for (size_t i = moor.x; i < moor.x + 3; i++)
			{
				for (size_t j = moor.y; j < moor.y + 3; j++)
				{
					if (world[i][j].isLive == 1)
					{
						k++;
					}
				}
			}
		}
		return k - 1;
	}

	// следующее поколение создается на основе проверки 
	//правил для старого: если количество соседей правильна, 
	//то клетка остается жить, нет - умерает

	Point nextGen(Point world[WIDTH][HEIGHT])
	{
		//Point temp[WIDTH][HEIGHT];
		// для каждой клетки нужно считать соседей
		//переберание и проверка каждой клетки на соседей
		// координаты для каждой клетки известны, это i и j
		for (size_t i = 0; i < WIDTH; i++)
		{
			for (size_t j = 0; j < HEIGHT; j++)
			{
				// считаем её соседей
				//world[i][j].isLive 
				Point pos;
				pos.x = i, pos.y = j;
				int k = count(pos, world);
				if (world[i][j].isLive == 1)
				{
					if (k < 2 || k > 3)
					{
						world[i][j].isLive = 0;
					}
					else
					{
						world[i][j].isLive = 1;
					}
				}
				else
				{
					if (k == 3)
					{
						world[i][j].isLive = 1;
					}
				}
			}
		}
		return world[WIDTH][HEIGHT];
	}

	// вывод поля на экран
	void pole(Point world[WIDTH][HEIGHT]) // функция получает информацию о клетках и выводит их на экран
	{
		system("cls");
		for (size_t i = 0; i < WIDTH; i++)
		{
			for (size_t j = 0; j < HEIGHT; j++)
			{
				if (world[i][j].isLive != 1)
				{
					cout << " ";
				}
				else
				{
					cout << "#";
				}
			}
			cout << endl;
		}
	}

	// копирование настоящего мира в предущий
	Point prevWorld(Point world[WIDTH][HEIGHT], Point oldWorld[WIDTH][HEIGHT])
	{
		for (size_t i = 0; i < WIDTH; i++)
		{
			for (size_t j = 0; j < HEIGHT; j++)
			{
				oldWorld[i][j] = world[i][j];
			}
		}
		return oldWorld[WIDTH][HEIGHT];
	}

	// сравнение предыдущего и настоящего мира 
	int compare(Point world[WIDTH][HEIGHT], Point oldWorld[WIDTH][HEIGHT])
	{
		for (size_t i = 0; i < WIDTH; i++)
		{
			for (size_t j = 0; j < HEIGHT; j++)
			{
				if (world[i][j].isLive != oldWorld[i][j].isLive)
					return -1;
			}
		}
		return 0;
	}

	//копирование текущего мира в массив конфигураций миров
	Point cmpWorldForKonfs(Point worldtek[WIDTH][HEIGHT], StabConf stUni[WORLDS], int& cycles)
	{

		for (size_t i = 0; i < WIDTH; i++)
		{
			for (size_t j = 0; j < HEIGHT; j++)
			{
				stUni[cycles].worlds[i][j] = worldtek[i][j];
			}
		}
		return stUni[cycles].worlds[WIDTH][HEIGHT];
	}

	//стабильная конфигурация не знаю как сделать
	// сравнивание текущей конфигурации с массивом конфигураций
	int stabilConf(Point world[WIDTH][HEIGHT], StabConf stUni[WORLDS], int cycles)
	{
		for (size_t i = 0; i <= cycles; i++)
		{
			if (compare(world, stUni[i].worlds) == 0)
			{
				return 0;
			}
		}
		return -1;
		
	}

	// очистка массива конфигураций 
	StabConf cleanUni(StabConf stUni[])
	{
		Point world[WIDTH][HEIGHT];
		for (size_t i = 0; i < WORLDS; i++)
		{
			stUni[i].worlds[WIDTH][HEIGHT] = world[WIDTH][HEIGHT];
		}
		return stUni[WORLDS];
	}

	// сохранение 
	void save(int gameTime, int kliving, int takts, int cyclyavnyj)
	{
		ofstream out("saves.txt", ios::app);
		time_t now = time(0);
		char* dt = ctime(&now);
		std::string line;
		(cyclyavnyj == 1) ? line = " да" : line = " нет";
		if (out.is_open())
		{
			out << dt <<
				" Время: " << gameTime <<
				" Живые клетки: " << kliving <<
				" Количество тактов: " << takts << 
				"\n" <<
				" Стабильная конфигурация: " << line << 
				"." << endl;
		}
		out.close();
	}

	// загрузка файла
	void load()
	{
		system("cls");
		gotoxy(0, 0);
		cout << "	______________MMMMMMMMMMMMM " << endl;
		cout << "	_____________MMMMMMMMMMMMMMMM" << endl;
		cout << "	____________MMMMMMMMMMMMMMMMMM" << endl;
		cout << "	___________MMM____MMMMMMMMMMMMMMMM" << endl;
		cout << "	___________MM______MMMMMMMMMMMMMMM" << endl;
		cout << "	___________MM_____MMMMMMMMMMMMMMMMM" << endl;
		cout << "	____________MM___MMMMMMMMMMMMMMMMM" << endl;
		cout << "	_____________MMMMMMMMMMMMMMMMMMMM" << endl;
		cout << "	______________MMMMMMMMMMMMMMMMM" << endl;
		cout << "	____MMMMM______MMMMMMMMMMMMMM_____MMMMMM" << endl;
		cout << "	__MMMMMMMMMM_____MMMMMMMMMM____MMMMMMMMMMM" << endl;
		cout << "	_MMMM_____MMMMMM___MMMMMM___MMMMMMMMMMMMMMM" << endl;
		cout << "	_MMM______MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM" << endl;
		cout << "	_MMM_MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM" << endl;
		cout << "	_MMMMMMMMMMMMMMMMMMMMMMMMM_MMMMMMMMMMMMMMMM" << endl;
		cout << "	__MMMMMMMMMMMMMMMMMMMM__MM___MMMMMMMMMMMMM" << endl;
		cout << "	_____MMMMMMMMMMMMMMM_M__MMM_____MMMMMMM" << endl;
		cout << "	_____MMMMMMMMMMMMMM__M___MMM______MMMMM" << endl;
		cout << "	_____MMMMMMMMMMMMM___MM___MM_______MMMMM" << endl;
		cout << "	_____MMMMMMMMMMMM____MMM____MMM_____MMM" << endl;
		cout << "	______MMMMMMMMMM_____MMMM____MMMMMMMMM" << endl;
		cout << "	________MMMMMM________MMMM_____MMMMM" << endl;
		cout << "	_________________________MM" << endl;
		std::string line;
		char buff[11];
		ifstream in("saves.txt");
		if (in.is_open())
		{
			int k = 0;
			while (getline(in, line))
			{
				gotoxy(55, k);
				k++;
				std::cout << line << std::endl;
				k++;
			}
		}
		in.close();
		system("pause");
	}
	
	int konec(int gameTime, int kliving, int takt, int cyclyavnyj)
	{		
		cout << "Записать результат?" << endl;
		Menu res;
		vector <string> textres = {
			"Да",
			"Нет"
		};
		int numres = res.select_vertical(textres, 1, 23);
		switch (numres)
		{
		case 0:
			system("cls");
			save(gameTime, kliving, takt, cyclyavnyj);
			return 1;
		default:
			return 1;
		}
	}

	int startGame(int prefTime, int cells,int dlitelnost = 100000, int taktyIgry = 100000)
	{
		Point world[WIDTH][HEIGHT];// текущая конфигурация
		Point oldWorld[WIDTH][HEIGHT]; // предыдущая конфигурация
		StabConf stabUniverse[WORLDS]; // массив конфигураций 
		/*world[WIDTH][HEIGHT] =*/ 	
		firstGen(world, cells); // инициация мира
		//pole(world);
		Sleep(400);
		int kliving = 0;//количество живых клеток
		int index = 0;// количетсво циклов 
		int takt = 0;
		int gameTime = 0; // время
		int optimastabil = 0; 
		do
		{
			if (_kbhit() == 0)
			{
				// логика симуляции	
				oldWorld[WIDTH][HEIGHT] = prevWorld(world, oldWorld); // предыдущая конфигурация превращается в текущую										
				stabUniverse[index].worlds[WIDTH][HEIGHT] = cmpWorldForKonfs(world, stabUniverse, index);// текущая конфигурация превращается в следующею
				world[WIDTH][HEIGHT] = nextGen(world); // присваивание одному из цикла значения мира 	
				kliving = countLive(world); // считать количество клеток
				//Sleep(1000);
				pole(world); // вывод на экран
				gameTime = clock() / CLOCKS_PER_SEC - prefTime;// время 
				cout << "Время:" << setw(3) << gameTime << setw(3) << fixed << setprecision(2) << "Живые клетки: " << setw(3) << kliving << fixed << setprecision(2) << "Такт: " << takt;
				Sleep(400);
				compare(world, oldWorld); // сравнение миров 
				// сравнивание текущей конфигурации с массивом конфигураций
				if (stabilConf(world, stabUniverse, index) == 0)
				{
					cout << "Стабильная система!";
					cout << endl;
					optimastabil = 1;
				}
				if (index < WORLDS)
				{
					index++;
					takt++;
				}
				else
					index = 0;
			}
			else
			{
				cout << endl << "Пауза" << endl;
				do
				{
					Menu start;
					vector <string> textmenu1 = {
						"Продолжить",
						"Выход"
					};
					int num = start.select_vertical(textmenu1, 1, 23);
					switch (num)
					{
					case 0:
						continue;
					default:
						cout << "Записать результат?" << endl;
						Menu res;
						vector <string> textres = {
							"Да",
							"Нет"
						};
						int numres = start.select_vertical(textres, 1, 28);
						switch (numres)
						{
						case 0:
							system("cls");
							save(gameTime, kliving, takt, optimastabil);
							return 1;
						default:
							return 1;
						}
					}
				} while (_kbhit() != 0);// пока не будет нажата кнопка 
			}
		} while (optimastabil != 1
			&& gameTime != dlitelnost 
			&& takt != taktyIgry 
			&& kliving != 0 
			&& compare(world, oldWorld) != 0);
		//stabUniverse[WORLDS] = cleanUni(stabUniverse);
		konec(gameTime, kliving, takt, optimastabil);
		return 1;
	}

	void preferences()
	{
		system("cls");
		cout << "0 - бесконечно" << endl;
		cout << "Длительность: ";
		int time;
		cin >> time;
		cout << "Количество тактов: ";
		int takt; 
		cin >> takt;
		if (time == 0 || takt == 0)
		{
			time = 100000;
			takt = 100000;
		}
		int kcells;
		cout << "Введите количество клеток, рекомендовано от 300, 0 - по умолчанию: ";
		cin >> kcells;
		if (kcells > WIDTH * HEIGHT)
		{
			do
			{
				cout << "Введите значение меньше чем " << WIDTH * HEIGHT << " ";
				cin >> kcells;
			} while (kcells > WIDTH * HEIGHT);	
		}
		int prefTime = clock() / CLOCKS_PER_SEC;

		startGame(prefTime, kcells, time, takt);
	}
};

void flower()
{
	SetColor(18, 0);
	cout << "	___________.░▒▒▒▒▒▒░. " << endl;
	cout << "	__________▒▒▒░▒░░░░▒▒." << endl;
	cout << "	__________░▒▒▒░▒▒▒░░▒▒" << endl;
	cout << "	_________░░░▒▒░░░▒░▒▒ " << endl;
	cout << "	____▓___░░░░░░`▒▒▒▒▒: " << endl;
	cout << "	____▓▓░░░░░░▒░░░░░░	  " << endl;
	cout << "	_ ▓_ ▓▓░░░░░▒░░░░░░░  " << endl;
	cout << "	_▓▓▓ ▓░░░░░▒░░░░░░░ ▓ " << endl;
	cout << "	_ ▓▓▓▓░░░░▒░░░░░░░▓▓  " << endl;
	cout << "	▓__▓▓▓░▓░▒░░░░░░▓▓    " << endl;
	cout << "	▓▓__'▓▓▓░▒░░░░▓▓▓     " << endl;
	cout << "	▓ ▓▓__▓▓'▓▓'▓▓▓▓´     " << endl;
	cout << "	▓▓ ▓▓▓____▓▓▓		  " << endl;
	cout << "	▓▓ ▓▓▓▓▓_ ▓▓		  " << endl;
	cout << "	▓▓▓ ▓▓▓▓__▓▓		  " << endl;
	cout << "	_▓▓▓ ▓▓▓__▓▓		  " << endl;
	cout << "	__▓▓▓ ▓▓▓_▓▓		  " << endl;
	cout << "	_____▓▓▓__▓▓		  " << endl;
	cout << "	_______▓▓_▓▓		  " << endl;
}

int main()
{
	srand(time(0));
	SetColor(14, 0);

	cout << "                                                  " << endl;
	cout << "                     181      181                 " << endl;
	cout << "             188888888  1888888   88888888111     " << endl;
	cout << "            1  11888888 888881888888888811118     " << endl;
	cout << "       18888888888  188 88   881111               " << endl;
	cout << "     181 1111888888    1       888888888888881    " << endl;
	cout << "     1188888888888   81811118 181  18888888888881 " << endl;
	cout << "   8888888888881   1   1   1   888881111          " << endl;
	cout << "  188       1118888  88  8  881   888888888       " << endl;
	cout << "  11     888888888 8888 188  88881  888188888     " << endl;
	cout << "     888888881  8888881188888 8888888  8888888    " << endl;
	cout << "    888881    18888888 888888  8888888    8888    " << endl;
	cout << "   888        8888888  8888888 88888888      88   " << endl;
	cout << "   1         18881 888 8888888    88888           " << endl;
	cout << "  88          88  88    88888      1881           " << endl;
	cout << "   88        18  88      8888       18            " << endl;
	cout << "   88888888  88 18        88        1             " << endl;
	cout << "   888811888    11                                " << endl;
	cout << "      888888888  8                                " << endl;
	cout << "        11111188 18                               " << endl;
	SetColor(8, 0);

	//flower();
	gotoxy(50, 8);
	cout << "Game of Live";
	cout << endl;
	gotoxy(50, 9);
	cout << "By Pavel Pavenskyy";
	cout << endl;
	gotoxy(30, 20);
	SetColor(2, 0);
	system("pause");

	setlocale(LC_ALL, "Russian");
	Game game;

	do
	{
		system("cls");
		void flower();
		Menu start;
		vector <string> textmenu1 = {
			"Начать симуляцию",
			"Результаты",
			"Выход"
		};
		int num = start.select_vertical(textmenu1, 40, 4);
		switch (num)
		{
		case 0:
			game.preferences();
			break;
		case 1:
			game.load();
			break;
		default:
			return 0;
		}
	} while (true);	

}
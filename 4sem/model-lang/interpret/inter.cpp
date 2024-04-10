#include <iostream>
#include <vector>
#include <stack>
using namespace std;


enum lexeme_type
{
	LEX_NULL,															// 0
	
	// Лексемы лексического анализа
	LEX_AND,															// 1
	LEX_BOOL,															// 2
	LEX_BREAK,															// 3
	LEX_CASE,															// 4
	LEX_CONTINUE,														// 5
	LEX_DO,																// 6
	LEX_ELSE,															// 7
	LEX_END,															// 8
	LEX_FALSE,															// 9
	LEX_FOR,															// 10
	LEX_GOTO,															// 11
	LEX_IF,																// 12
	LEX_INT,															// 13
	LEX_NOT,															// 14
	LEX_OF,																// 15
	LEX_OR,																// 16
	LEX_PROGRAM,														// 17
	LEX_READ,															// 18
	LEX_STRING,															// 19
	LEX_TRUE,															// 20
	LEX_WHILE,															// 21
	LEX_WRITE,															// 22
	LEX_WRITELINE,														// 23
	LEX_FIN,															// 24
	
	LEX_LEFT_BRACE,														// 25   1
	LEX_RIGHT_BRACE,													// 26   2
	LEX_QUOTE,															// 27   3
	LEX_SEMICOLON,														// 28   4
	LEX_COMMA,															// 29   5
	LEX_COLON,															// 30   6
	LEX_ASSIGN,															// 31   7
	LEX_PLUS,															// 32   8
	LEX_MINUS,															// 33   9
	LEX_TIMES,															// 34   10
	LEX_SLASH,															// 35   11
	LEX_PERCENT,														// 36   12
	LEX_PLUS_PLUS,														// 37   13
	LEX_MINUS_MINUS,													// 38   14
	LEX_PLUS_ASSIGN,													// 39   15
	LEX_MINUS_ASSIGN,													// 40   16
	LEX_LEFT_PAREN,														// 41   17
	LEX_RIGHT_PAREN,													// 42   18
	LEX_EQ,																// 43   19
	LEX_GREATER,														// 44   20
	LEX_LESS,															// 45   21
	LEX_GREATER_EQ,														// 46   22
	LEX_LESS_EQ,														// 47   23
	LEX_NOT_EQ,															// 48   24

	LEX_ID,																// 49
	LEX_NUM,															// 50
	LEX_STR_CONST,														// 51

	// Лексемы синтаксического анализа
	LEX_UNARY_MINUS,													// 52 - унарный минус (определяется путем синтаксического анализа)
	LEX_PP_PRE,															// 53 - префиксный '++'
	LEX_PP_POST,														// 54 - постфиксный '++'
	LEX_MM_PRE,															// 55 - префиксный '--'
	LEX_MM_POST,														// 56 - постфиксный '--'
	
	POLIZ_GO,															// 57
	POLIZ_FGO,															// 58
	POLIZ_LABEL,														// 59
	POLIZ_ADDRESS														// 60
};


class Lexeme
{
	lexeme_type lex_type;
	int value;
	
public:
	Lexeme(lexeme_type t = LEX_NULL, int val = 0): lex_type(t), value(val){}

	lexeme_type type() const
	{ return lex_type; }
	
	int val() const
	{ return value; }

	friend ostream &operator<<(ostream &out, Lexeme Lex)
	{
		out << '(' << Lex.lex_type << ',' << Lex.value << "); ";
		return out;
	}
};


class ID
{
	string name;
	bool declare;
	lexeme_type type;
	bool assign;
	int value;
	string str_value;
	bool label;
	int address;

public:
	ID(const string str = "\0"): declare(false), assign(false), label(false)
	{ if(!str.empty()) name = str; value = -1; address = -1; }
	
	string get_name(){ return name; }
	
	void set_name (const string new_name){ name = new_name; }
	
	bool get_declare(){ return declare; }
	
	void set_declare(){ declare = true; }
	
	lexeme_type get_type(){ return type; }
	
	void set_type(lexeme_type t){ type = t; }
	
	bool get_assign(){ return assign; }
	
	void set_assign(){ assign = true; }
	
	int get_value(){ return value; }
	
	string get_str_value(){ return str_value; }
	
	void set_value(int new_val){ value = new_val; }
	
	void set_value(string new_val){ str_value = new_val; }
	
	bool is_label(){ return label; }
	
	void set_label(){ label = true; declare = true; }

	int get_address(){ return address; }

	void set_address(int new_val){ address = new_val; }
};


class Scanner															// КЛАСС: СКАНЕР
{
	FILE *fp;															//    дескриптор файла программы на модельном языке
	enum state
	{
		H,																//    начальное состояние
		IDENT,
		NUMBER,
		STRING,
		COMMENT,
		COMMENT_STRING,
		DELIM,
		NOT_EQ,
		FIN
	};
	state CS;															//    актуальное состояние
	
	static string Word_Table[];											//    таблица служебных слов
	static lexeme_type words[];
	
	static string Delim_Table[];										//    таблица разделителей
	static lexeme_type delims[];
	
	char c;
	string buf;															//    буфер для хранения вводимой строки
	int buf_size = 80;													//    размер буфера (макс. количество символов)
	int buf_top;														//    номер последнего непустого символа в буфере
	
private:
	void plug_program_file(const string str)							//    Открытие файла программы для чтения
	{ fp = fopen(str.c_str(), "r"); return; }
	
	void clear()														//    Очистка буфера
	{ buf_top = 0; buf.clear(); return; }
	
	void add(char x)													//    Запись нового элемента в буфер
	{ buf.push_back(x); buf_top++; return; }
	
	int look(const string &buf, const string list[])					//    Проверка наличия строки, находящейся в буфере, в списке лексем
	{
		int i = 0;
		while(!list[i].empty())
		{
			if(buf == list[i])											//      если строка совпадает с i-ым элементом списка
				return i+1;												//        возвращаем номер этого элемента
			i++;														//        иначе переходим к следующему
		}
		return 0;														//      возвращаем 0 в случае несоответствия строки из буфера ни одному элементу списка
	}
	
	void gc()															//    Cчитываниe очередного символа программы на модельном языке
	{ c = fgetc(fp); return; }
	
	void ungc()
	{ ungetc(c, fp); return; }
	
	void lex_error(string err)											// Обработка лексической ошибки
	{
		try
		{ throw err; }
		catch(string s)
		{
			string err_des;
			switch(s[0])
			{
				case '\\':
					err_des = "1: Unknown escape sequence: " + s;
					break;
				case '\"':
					err_des = "2: Missing terminating \" character.";
					break;
				case '!':
					err_des = "3: Expected initializer before '" + s + "' token.";
					break;
				case '\'':
					err_des = "4: No match for deliminator " + s;
					break;
			}
			cerr << "LEXICAL ERROR #" << err_des << endl;
			exit(1);
		}
	}

public:
	Scanner(string prog_name)
	{ plug_program_file(prog_name); CS = H; gc(); }
	
	~Scanner()
	{ fclose(fp); }
	
	Lexeme get_lexeme();
};


string Scanner::Word_Table[] =
{
	"and",
	"bool",
	"break",
	"case",
	"continue",
	"do",
	"else",
	"end",
	"false",
	"for",
	"goto",
	"if",
	"int",
	"not",
	"of",
	"or",
	"program",
	"read",
	"string",
	"true",
	"while",
	"write",
	"writeline",
};

lexeme_type Scanner::words[] =
{
	LEX_AND,															// 1
	LEX_BOOL,															// 2
	LEX_BREAK,															// 3
	LEX_CASE,															// 4
	LEX_CONTINUE,														// 5
	LEX_DO,																// 6
	LEX_ELSE,															// 7
	LEX_END,															// 8
	LEX_FALSE,															// 9
	LEX_FOR,															// 10
	LEX_GOTO,															// 11
	LEX_IF,																// 12
	LEX_INT,															// 13
	LEX_NOT,															// 14
	LEX_OF,																// 15
	LEX_OR,																// 16
	LEX_PROGRAM,														// 17
	LEX_READ,															// 18
	LEX_STRING,															// 19
	LEX_TRUE,															// 20
	LEX_WHILE,															// 21
	LEX_WRITE,															// 22
	LEX_WRITELINE,														// 23
	LEX_FIN,															// 24
};

string Scanner:: Delim_Table[] =
{
	"{",
	"}",
	"\"",
	";",
	",",
	":",
	"=",
	"+",
	"-",
	"*",
	"/",
	"%",
	"++",
	"--",
	"+=",
	"-=",	
	"(",
	")",
	"==",
	">",
	"<",
	">=",
	"<=",
	"!=",
};

lexeme_type Scanner::delims[] =
{
	LEX_LEFT_BRACE,														// 1
	LEX_RIGHT_BRACE,													// 2
	LEX_QUOTE,															// 3
	LEX_SEMICOLON,														// 4
	LEX_COMMA,															// 5
	LEX_COLON,															// 6
	LEX_ASSIGN,															// 7
	LEX_PLUS,															// 8
	LEX_MINUS,															// 9
	LEX_TIMES,															// 10
	LEX_SLASH,															// 11
	LEX_PERCENT,														// 12
	LEX_PLUS_PLUS,														// 13
	LEX_MINUS_MINUS,													// 14
	LEX_PLUS_ASSIGN,													// 15
	LEX_MINUS_ASSIGN,													// 16
	LEX_LEFT_PAREN,														// 17
	LEX_RIGHT_PAREN,													// 18
	LEX_EQ,																// 19
	LEX_GREATER,														// 20
	LEX_LESS,															// 21
	LEX_GREATER_EQ,														// 22
	LEX_LESS_EQ,														// 23
	LEX_NOT_EQ,															// 24
};

vector <ID> ID_Table;													// Вектор (таблица идентификаторов)

vector <string> STR_CONST_Table;										// Вектор (таблица строковых констант)

int put(const string &buf)												// Функция поиска по таблице идентификаторов и ее заполнения
{
	vector <ID>::iterator it;
	for(it = ID_Table.begin(); it != ID_Table.end(); ++it)
		if(buf == it->get_name())										//    если идентификатор с данным названием уже находится в таблице, то
			return it - ID_Table.begin();								//      возвращаем номер идентификатора в таблице
	
	ID_Table.push_back(ID(buf));										//    иначе добавляем заданный идентификатор в конец таблицы
	return ID_Table.size() - 1;											//    и возвращаем номер идентификатора в таблице
}

int put_string(const string &buf)										// Функция поиска cтроки по таблице строковых констант и ее заполнения
{
	vector <string>::iterator it;
	for(it = STR_CONST_Table.begin(); it != STR_CONST_Table.end(); ++it)
		if(buf == *it)													//    если данная строка уже находится в таблице, то
			return it - STR_CONST_Table.begin();						//      возвращаем номер этой строки в таблице
	
	STR_CONST_Table.push_back(buf);										//    иначе добавляем эту строку в конец таблицы
	return STR_CONST_Table.size() - 1;									//    и возвращаем ее номер в таблице
}


Lexeme Scanner::get_lexeme()
{
	clear();
	int d, i;															// d - число, встретившееся в программе; i - значение очередной лексемы 
	do
	{
		switch(CS)
		{
			case H:														// Начальное состояние:
				if(c == ' ' || c == '\n' || c == '\r' || c == '\t') gc();//   если очередной символ - пробел/конец строки/признак начала новой строки, то считываем следующий символ
				
				else if(isalpha(c))										//    если очередной символ - буква, то
				{ clear(); add(c); gc(); CS = IDENT; }					//      очищаем  буфер, добавляем символ, переходим в состояние идентификатора, считываем следующий символ
				
				else if(isdigit(c))										//    если очередной символ - число, то
				{ d = c - '0'; gc(); CS = NUMBER; }
				
				else if(c == '\"')
				{
					add(c);
					gc();
					CS = STRING;
					i = look(buf, Delim_Table);
					return Lexeme(LEX_QUOTE, i);
				}     
				
				else if (c == '/')
				{
					clear();
					add(c);
					gc();
					switch(c)
					{
						case '*':
							clear();
							gc();
							CS = COMMENT;
							break;
						case '/':
							clear();
							gc();
							CS = COMMENT_STRING;
							break;
						default:										//    в случае, если это не комментарий:
							ungc();										//      возвращаем считанный символ обратно в файл
							c = '/';									//		задаем c изначальный символ: '/'
							CS = DELIM;
							break;
					}
				}
				else if (c == '!')
				{ clear(); add(c); gc(); CS = NOT_EQ; }
				
				else if(c == EOF)
					CS = FIN;
				
				else
				{ clear(); add(c); CS = DELIM; }
				
				break;
			
			case IDENT:													// Идентификатор
				i = look(buf, Word_Table);
				if((isalpha(c)) || (isdigit(c)))
				{ add(c); gc(); }										//    добавление последующих сиимволов
				else 
				{
					CS = H;												//    перед завершением анализа лексемы нужно перейти в начальное состояние
					if(i)												//    если строка, находящаяся в буфере, совпадает с одним из элементов таблицы служебных слов, то
						return Lexeme((lexeme_type)i, i);				//      возвращаем лексему
					else
					{ i = put(buf); return Lexeme(LEX_ID, i); }			//    иначе добавляем строку в список служебных слов
				}
				break;
			
			case NUMBER:
				if(isdigit(c))
				{ d = 10*d + (c - '0'); gc(); }
				else
				{ CS = H; return Lexeme(LEX_NUM, d); }					//    возвращаем лексему
				break;
				
			case STRING:												// Строка
				clear();												//    очистка буфера для записи содержимого строки
				if(c != '\"')											//    если очередной символ - не завершающие ковычки ", то
				{
					while(c != '\"')									//      работаем с содержимым строки до встречи "
					{
						if(c == '\\')									//      если с - управляющий символ, то
						{
							gc();
							switch(c)									//        проверка следующего символа
							{
								case 'n':								//          символ начала новой строки
									add('\n');
									break;
								case '0':								//			символ конца строки
									add('\0');
									break;
								case 'r':								//			возврат каретки
									add('\r');
									break;
								case 't':								//			горизонтальная табуляция
									add('\t');
									break;
								case '\\': case '\'': case '\"': case '\?': case '\%'://символы
									add(c);
									break;
								case '\n':								//			продолжение строки в следующей строке программного кода
									gc();
									while(c == '\t')					//			пока следующий символ - не отступ, используемый для выравнивания программного кода	
										gc();							//			  берем новый символ
									ungc();								//			возвращаем символ, не являющийся отступом, обратно в программу
									break;
								default:								//        неверный управляющий символ
									string err = "\\" + string(1,c);
									lex_error(err);
									break;
							}
						}
						else if(c == '\n')								//      иначе, если с - символ конца строки
							lex_error("\"");							//        лексическая ошибка(из-за отсутствия завершающих ковычек)
						else											//      в остальных случаях с - элемент строки
							add(c);										//        поэтому добавляем его в буфер
						gc();
					}
					i = put_string(buf);								//      при нахождении " добавляем буфер с полученной строкой в таблицу идентификаторов
					return Lexeme(LEX_STR_CONST, i);
				}
				else													//    если очередной символ - завершающие ковычки, то
				{
					add(c);
					gc();
					CS = H;												//      выходим из состояния строки
					i = look(buf, Delim_Table);
					return Lexeme(LEX_QUOTE, i);						//      добавляем завершающие ковычки в таблицу идентификаторов
				}
				break;
				
			case COMMENT:
				add(c);
				if(c == '*')
				{
					gc();
					if(c == '/')
					{ add(c); gc(); CS = H; }
					else
						ungc();
				}
				if(c == EOF)
					CS = FIN;
				else
					gc();
				break;
			
			case COMMENT_STRING:
				while(c != '\n' && c != EOF)
					gc();
				if(c == EOF)
					CS = FIN;
				else
				{ gc(); CS = H; }
				break;
			
			case NOT_EQ:
				if (c == '=')
				{
					add(c);
					gc();
					CS = H;
					i = look(buf, Delim_Table);
					return Lexeme(LEX_NOT_EQ, i);
				}
				else
					lex_error("!");
				break;
			
			case DELIM:
				char c1; c1 = c;
				gc();
				
				// анализ составных разделителей ("++", "--", "+=", "-=", "==", ">=", "<=")
				if(c1 == '+' || c1 == '-' || c1 == '>' ||				// проверка первого символа составного разделителя (c1)
				   c1 == '<' || c1 == '=')
				switch(c)												// switch происходит по второму символу
				{
					case '+': case '-':
						if(c1 == c)										// проверка того, что составной символ - либо "++", либо "--"
						{ add(c); gc(); }
						break;
					case '=':
						add(c);											// '=' может стоять после любого символа c1 составного разделителя
						gc();
						break;
					default:
						break;
				}
				
				CS = H;
				i = look(buf, Delim_Table);
				if(i)
					return Lexeme((lexeme_type)(i+(int)LEX_FIN), i);
				else
					lex_error("'" + buf + "'");
				break;
				
			case FIN:
//				CS = H;
				return Lexeme(LEX_FIN);
				break;
		}
	}
	while (true);
};


class Parser															// КЛАСС: ПАРСЕР (ДЛЯ СИНТАКСИЧЕСКОГО АНАЛИЗА)
{
	Lexeme curr_lex;
	lexeme_type curr_type;
	int curr_val;
	
	Scanner Scan;
	stack <lexeme_type> lex_stack;
	
	bool loop;															// Индикатор того, что программа находится в состоянии цикла 
	int nested_loops;													// Количество вложенных циклов
	bool lvalue;
	
	class break_stack_elem
	{
	public:
		int nl_num;
		int pos;
		break_stack_elem(int x = 0, int y = 0): nl_num(x), pos(y){}
	};
	
	stack <break_stack_elem> break_stack;								// Cтек операторов break (элемент стека состоит из номера метки в полизе и номера вложенного цикла, в котором находится break)
	
	stack <int> plus_stack;
	stack <int> minus_stack;
	stack <int> lvalue_uncertain_stack;
	int num;
	
	bool pp_id;
	
	// Cинтаксические действия / терминалы грамматики исполняемой программы
	void P();								// Цель						P	     --> program{OPS}      (описания переменных входят в множество операторов)

	void DES();								// Описания					DES      --> D; DES | D; | eps
	void D();								// Описание					D        --> [int | string | bool] I <, I>			
	void I();								// Переменная				I	     --> LEX_ID | LEX_ID = CONST														
	void CONST();							// Константа				CONST    --> INT | STR | BOOL

	void OPS();								// Операторы				OPS      --> <OP>
	void OP();								// Оператор					OP       --> DES | OP_STMNT | {OPS} | if(STMNT) OP <else OP> | while(STMNT) OP | for([STMNT]; [STMNT]; [STMNT]) OP | break; | goto LABEL; | read(ID); | write(STMNT <, STMNT>);
	void OP_STMNT();						// Оператор-выражение		OP_STMNT --> STMNT ;
	void STMNT(int x = 1);					// Выражение				STMNT    --> A | A = STMNT | A [==|<|>|<=|>=|!=] A 
	void A();								// 							A        --> B | B [+|-|or] B
	void B();								//							B	     --> C | C [*|/|and] C
	void C();								//							C	     --> ID | LABEL: | ID++ | ID-- | ++ID | --ID | [+|-] C | STR | BOOL | not C | STMNT
	
	// семантические действия
	void dec();
	void check_id(int val);
	void check_id_in_read();
	void check_op();
	void check_unary_op();
	void check_not();
	void eq_type();
	void eq_bool();
	void break_controller(string mode);
	void check_break();
	void check_goto();
	
	// преобразование в полиз
	void unary_op_convertion();
	
	void gl()															// Получение очередной лексемы
	{
		curr_lex = Scan.get_lexeme();									//    сканер получает очередную лексему
		curr_type = curr_lex.type();									//    получаем тип очередной лексемы
		curr_val = curr_lex.val();										//    получаем значение очередной лексемы
	}
	
	void syn_error(string err_des)										// Обработка синтаксической ошибки
	{
		try
		{ throw err_des; }
		catch(string s)
		{ cerr << "SYNTAX ERROR #" << s << ". Lexeme: " << curr_lex << endl; exit(1); }
	}
	
	void sem_error(string err_des)										// Обработка семантической ошибки
	{
		try
		{ throw err_des; }
		catch(string s)
		{ cerr << "ERROR: " << s << endl; exit(1); }
	}
	
	void sem_warning(string err_des)									// Обработка семантического предупреждения
	{
		try
		{ throw err_des; }
		catch(string s)
		{ cerr << "WARNING: " << s << endl << endl; }
	}
	
public:
	Parser(const string prog_name): Scan(prog_name)
	{ loop = 0; nested_loops = -1; lvalue = 1; pp_id = 0; }
	
	vector <Lexeme> Poliz;
	
	void poliz_print()													// Функция поэлементного вывода полиза программы
	{
		vector<Lexeme>::iterator it;
		cout << "{ ";
		for(it = Poliz.begin(); it != Poliz.end(); ++it)
			cout << *it << " ";
		cout << "}\n";
	}
	
	void analyse();
};


template <class T1, class T2>
void extract(T1 &t, T2 &x)												// Извлечение элементов стека
{ x = t.top(); t.pop(); }


void Parser::analyse()
{
	gl();
	P();
	if(curr_type != LEX_FIN)
		syn_error("1: No final found...how is this even possible?");	//#1
	cout << "Your program is just fine.\n\n";
}

// СИНТАКСИЧЕСКИЙ АНАЛИЗ
void Parser::P()														// Цель
{
	if(curr_type == LEX_PROGRAM)										//    если первая лексема - "program", то
		gl();															//      получаем следующую лексему
	else 																//    иначе ошибка
		syn_error("2: Did you forget 'program'?");						//#2
	if(curr_type == LEX_LEFT_BRACE)
		gl();
	else
		syn_error("3: Did you forget '{'?");							//#3
	OPS();																//    разбор операторов
	check_goto();
}

void Parser::DES()														// Описания
{
	D();																//      разбор отдельного описания
	if(curr_type != LEX_SEMICOLON)										//      если лексема, следующая после описания - не ";", то это ошибка
		syn_error("4: Did you forget ';'?");							//#4
	gl();
}

void Parser::D()														// Описание
{
	lexeme_type ID_type = curr_type;									//   сохраняем тип переменной, которая будет описываться далее (для задания его последующей переменной и на случай присваивания ей некторого значения)
	
	lex_stack.push(ID_type);											//   добавляем его в стек
	gl();
	I();																//    разбор переменной
	while(curr_type == LEX_COMMA)										//    до тех пор, пока следующая лексема - "," (т.е. пока есть еще переменные этого же типа)
	{
		lex_stack.push(ID_type);										//      снова сохраняем в стеке заданный тип переменной
		gl();
		I();															//      разбор следующей переменной того же типа
	}
}

void Parser::I()														// Переменная
{
	if(curr_type == LEX_ID)												
	{
		dec();															//    задаем очередной лексеме тип данных, объявленный ранее (он был сохранен в стеке 12 строчками выше)
		Poliz.push_back(Lexeme(POLIZ_ADDRESS, curr_val));
		gl();
		
		if(curr_type == LEX_ASSIGN)
		{ gl(); CONST(); Poliz.push_back(LEX_ASSIGN); }
		else
			Poliz.pop_back();
		
		if(curr_type == LEX_COMMA || curr_type == LEX_SEMICOLON)		//    если следующая лексема - "," или ";", то
			lex_stack.pop();											//		удаление из стека типа переменной и возврат из разбора отдельной переменной
		else
			syn_error("5: Unallowed deliminator. Only '=' available");	//#5
	}
	else
		syn_error("6: No Identificator found");							//#6
}

void Parser::CONST()													// Константа
{
	if(lex_stack.top() == LEX_INT || lex_stack.top() == LEX_STRING ||
	   lex_stack.top() == LEX_BOOL)
	{ STMNT(0); eq_type(); }
	else
		syn_error("7: No matching const type found");					//#7
}

void Parser::OPS()														// Операторы
{
	while(curr_type != LEX_RIGHT_BRACE)
	{
		OP();
		if(curr_type == LEX_FIN)
			syn_error("8: Did you forget '}'?");						//#8
	}
	gl();
}

void Parser::OP()														// Оператор
{
	int p0, p1, p2, p3, p4;
	
	switch(curr_type)
	{
		case LEX_INT: case LEX_BOOL: case LEX_STRING:					// Описания идентификаторов
			DES();
			break;
			
		case LEX_IF:													// Оператор if()
			gl();
			if(curr_type != LEX_LEFT_PAREN)
				syn_error("9: 'if' expression: expected '(' after 'if'");//#9
			gl();
			STMNT();
			eq_bool();

			p2 = Poliz.size();
			Poliz.push_back(Lexeme());
			Poliz.push_back(Lexeme(POLIZ_FGO));
			
			if(curr_type != LEX_RIGHT_PAREN)
				syn_error("10: 'if' expression: did you forget ')' ?");	//#10
			gl();
			OP();
			Poliz[p2] = Lexeme(POLIZ_LABEL, Poliz.size());
			
			if(curr_type == LEX_ELSE)
			{
				p3 = Poliz.size();
				Poliz.push_back(Lexeme());
				Poliz.push_back(Lexeme(POLIZ_GO));
				Poliz[p2] = Lexeme(POLIZ_LABEL, Poliz.size());
				gl();
				OP();
				Poliz[p3] = Lexeme(POLIZ_LABEL, Poliz.size());
			}
			break;
		
		case LEX_WHILE:													// Цикл while()
			p0 = Poliz.size();
			gl();
			if(curr_type != LEX_LEFT_PAREN)
				syn_error("11: 'while' expression: expected '(' after 'while'");//#11
			gl();
			STMNT();
			eq_bool();
			p1 = Poliz.size(); 
			Poliz.push_back(Lexeme());
			Poliz.push_back(Lexeme(POLIZ_FGO));
			
			if(curr_type != LEX_RIGHT_PAREN)
				syn_error("12: 'while' expression: did you forget ')' ?");//#12
			break_controller("on");
			gl();
			OP();
			
			Poliz.push_back(Lexeme(POLIZ_LABEL, p0));
            Poliz.push_back(Lexeme(POLIZ_GO));
            Poliz[p1] = Lexeme(POLIZ_LABEL, Poliz.size());
            
            break_controller("off");									//    обработка операторов break в полизе также происходит в функции break_switch("off")
			break;
		
		case LEX_FOR:													// Цикл for(;;)
			gl();
			if(curr_type != LEX_LEFT_PAREN)
				syn_error("13: 'for' expression: expected '(' after 'for'");//#13
			gl();														//    for(вот это; ...; ...)
			if(curr_type == LEX_SEMICOLON)
				gl();
			else if(curr_type == LEX_INT || curr_type == LEX_BOOL || curr_type == LEX_STRING)
				DES();
			else														//    может быть либо инициализацией (с присваиванием!)
				OP_STMNT();												//    либо выражением
			
			p3 = Poliz.size();
																		//    for(...; вот это; ...)
			if(curr_type == LEX_SEMICOLON)	
			{ Poliz.push_back(Lexeme(LEX_TRUE, 1)); gl(); } 
			else
			{
				STMNT();
				eq_bool();
				if(curr_type != LEX_SEMICOLON)
					syn_error("14: 'for' expression: ';' between last two statements is missing");//#14
				gl();
			}
			
			p1 = Poliz.size();
			Poliz.push_back(Lexeme());
			Poliz.push_back(Lexeme(POLIZ_FGO));
			
			p2 = Poliz.size();
			Poliz.push_back(Lexeme());
			Poliz.push_back(Lexeme(POLIZ_GO));
			p4 = Poliz.size();
																		//    for(...; ...; вот это)
			if(curr_type == LEX_RIGHT_PAREN)
				gl();
			else
			{
				STMNT();
			
				Poliz.push_back(Lexeme(POLIZ_LABEL, p3));
				Poliz.push_back(Lexeme(POLIZ_GO));
			
				if(curr_type != LEX_RIGHT_PAREN)
					syn_error("15: 'for' expression: did you forget ')' ?");//#15
				gl();
			}
			Poliz[p2] = Lexeme(POLIZ_LABEL, Poliz.size());
			
			break_controller("on");
			OP();
			
			Poliz.push_back(Lexeme(POLIZ_LABEL, p4));
			Poliz.push_back(Lexeme(POLIZ_GO));
			Poliz[p1] = Lexeme(POLIZ_LABEL, Poliz.size());
			
			break_controller("off");									//    обработка операторов break в полизе также происходит в функции break_switch("off")
			break;
		
		case LEX_BREAK:													// Оператор break
			check_break();
			gl();
			if(curr_type != LEX_SEMICOLON)
				syn_error("16: did you forget ';' ?");					//#16
			gl();
			break;
		
		case LEX_GOTO:													// Оператор goto
			gl();
			if(curr_type != LEX_ID)
				syn_error("17: expected label after \"goto\" operator");//#17
			if(!ID_Table[curr_val].is_label())							//    если идентификатор не обЪявлен как метка, то
				if(!ID_Table[curr_val].get_declare())					//      если идентификатор не обЪявлен вообще, то
				{														//      объявляем его как метку (это значит, что такая метка не встречалась ранее)
					ID_Table[curr_val].set_label();
					ID_Table[curr_val].set_address(Poliz.size());
					Poliz.push_back(Lexeme());
					Poliz.push_back(Lexeme(POLIZ_GO));
				}
				else 													//    иначе идентификатор уже объявлен (как переменная), то ошибка
					syn_error("18: the identificator has already been declared");//#18
			else 														//    если он уже обЪявлен как метка, то эта метка была объявлена ранее в программном коде
			{
				int val = ID_Table[curr_val].get_value();
				Poliz.push_back(Lexeme(POLIZ_LABEL, val));
				Poliz.push_back(Lexeme(POLIZ_GO));
			}
			gl();
			if(curr_type != LEX_SEMICOLON)
				syn_error("19: \"goto\" operator: did you forget ';' ?");//#19
			gl();
			break;
		
		case LEX_READ:													// Оператор read()
			gl();
			if(curr_type != LEX_LEFT_PAREN)
				syn_error("20: 'read' expression: expected '(' after 'read'");//#20
			gl();
			if(curr_type != LEX_ID)
				syn_error("21: 'read' expression: identificator not found");//#21
			check_id_in_read();
			Poliz.push_back(Lexeme(POLIZ_ADDRESS, curr_val));
			gl();
			
			if(curr_type != LEX_RIGHT_PAREN)
				syn_error("22: 'read' expression: did you forget ')' ?");//22
			gl();
			Poliz.push_back(Lexeme(LEX_READ));
				
			if(curr_type != LEX_SEMICOLON)
				syn_error("23: Did you forget ';' ?");					//#23
			gl();
			break;
		
		case LEX_WRITE:	case LEX_WRITELINE:								// Операторы write() и writeline()
		{
			lexeme_type write_mode = curr_type;
			gl();
			if(curr_type != LEX_LEFT_PAREN)
				syn_error("24: 'write' expression: expected '(' after 'write'");//#24
			gl();
			if(curr_type == LEX_RIGHT_PAREN)
				syn_error("25: 'write' expression: identificator not found");//#25
			STMNT(0);
			while(curr_type == LEX_COMMA)
			{ gl(); STMNT(0); }
			
			if(curr_type != LEX_RIGHT_PAREN)
				syn_error("26: 'write' expression: did you forget ')' ?");//#26
			gl();
			Poliz.push_back(Lexeme(write_mode));
			if(curr_type != LEX_SEMICOLON)
				syn_error("27: Did you forget ';' ?");					//#27
			gl();
			break;
		}
		case LEX_LEFT_BRACE:											// Составной оператор
			gl();
			OPS();
			break;
		
		default:														// Оператор-выражение
			OP_STMNT();
			break;
	}
}

void Parser::OP_STMNT()													// Оператор - выражение
{
	pp_id = 0;
	STMNT(1);
	if(curr_type == LEX_SEMICOLON || curr_type == LEX_COLON)
		gl();
	else
		syn_error("28: Did you forget ';' ?");							//#28
}

void Parser::STMNT(int x)												// Выражение
{
	lvalue = x;
	lexeme_type assign_type = curr_type;								//    сохраняем тип первой лексемы (на случай присваивания)
	A();
	
	if(curr_type == LEX_ASSIGN)											//    если происходит присваивание, то
		if(assign_type == LEX_ID && lvalue)								//      проверяем, что до '=' был идентификатор - lvalue выражение
		{
			pp_id = 1;
			int val;
			extract(lvalue_uncertain_stack, val);
			Poliz[num] = Lexeme(POLIZ_ADDRESS, val);
			gl();
			STMNT();
			eq_type();
			Poliz.push_back(LEX_ASSIGN);
			unary_op_convertion();
		}
		else
			syn_error("29: Lvalue required as a left operand of assignment");//#29
	
	else if(curr_type >= LEX_EQ && curr_type <= LEX_NOT_EQ)
	{
		lexeme_type compare_type = curr_type;
		
		lvalue = 0;														//    для выражения в левой части оператора присваивания: если в левой части встречаются любые операторы
		lex_stack.push(curr_type); 
		gl();
		A();
		check_op();
		Poliz.push_back(Lexeme(compare_type));
	}
	
	if(!lvalue_uncertain_stack.empty())
	{
		int val;
		extract(lvalue_uncertain_stack, val);
		lvalue ? Poliz[num] = Lexeme(POLIZ_ADDRESS, val) : Poliz[num] = Lexeme(LEX_ID, val);
	}
	
	if(!pp_id)
		unary_op_convertion();
}

void Parser::A()
{
	B();
	while(curr_type == LEX_PLUS || curr_type == LEX_MINUS || curr_type == LEX_OR)
	{
		lexeme_type additive_type = curr_type;
		
		lvalue = 0;														//    для выражения в левой части оператора присваивания: если в левой части встречаются любые операторы
		lex_stack.push(curr_type);
		gl();
		B();
		check_op();
		Poliz.push_back(Lexeme(additive_type));
	}
}

void Parser::B()
{
	C();
	while(curr_type == LEX_TIMES || curr_type == LEX_SLASH ||
		  curr_type == LEX_PERCENT || curr_type == LEX_AND)
	{
		lexeme_type multi_type = curr_type;
		
		lvalue = 0;														//    для выражения в левой части оператора присваивания: если в левой части встречаются любые операторы
		lex_stack.push(curr_type);
		gl();
		C();
		check_op();
		Poliz.push_back(Lexeme(multi_type));
	}
}

void Parser::C()
{
	switch(curr_type)
	{
		case LEX_ID:
		{
			if(lvalue)
			{
				lvalue_uncertain_stack.push(curr_val);
				num = Poliz.size();
				Poliz.push_back(Lexeme());
			}
			else
				Poliz.push_back(Lexeme(LEX_ID, curr_val));
			
			int id_val = curr_val;
			gl();
			if(curr_type == LEX_COLON)									//    если за идентификатором следует ':', то данный  идентификатор - метка
				if(!pp_id)
				{
					if(lvalue)
						lvalue_uncertain_stack.pop();
					Poliz.pop_back();
					
					if(ID_Table[id_val].is_label())						//      если метка была объявлена ранее, то					
					{
						int pos = ID_Table[id_val].get_address();		//        pos - адрес нахождения метки в полизе
						if(ID_Table[id_val].get_value() != -1)			//		  если значение метки уже задано, то это значит, что метка была поставлена дважды в программе
							syn_error("30: Label \"" + ID_Table[id_val].get_name() + "\" is declared twice");//#30
						
						ID_Table[id_val].set_value(Poliz.size());		//		  задаем значение, в которое будет направлять данная метка
						ID_Table[id_val].set_assign();					//        утверждаем, что значение метки задано
						Poliz[pos] = Lexeme(POLIZ_LABEL, Poliz.size());
					}
					else if(!ID_Table[id_val].get_declare())			//      если метка не была объявлена и не является идентификатором (!), то		
					{
						ID_Table[id_val].set_label();					//		  объявляем идентификатор как метку
						ID_Table[id_val].set_value(Poliz.size());		//        задаем метке значение, в которое метка будет направлять ход программы
						ID_Table[id_val].set_assign();					//        утверждаем, что значение метки задано
					}
					else
						syn_error("31: Label \"" + ID_Table[id_val].get_name() + "\" is already declared as an identificator and cannot be used");//#31
				}
				else
					syn_error("32: Wrong usage of label \"" + ID_Table[id_val].get_name() + "\"");//#32
			
			else if(curr_type == LEX_PLUS_PLUS || curr_type == LEX_MINUS_MINUS)
			{
				if(!pp_id)
					Poliz.pop_back();
				lvalue = 0;
				curr_type == LEX_PLUS_PLUS ? plus_stack.push(id_val) : minus_stack.push(id_val);
				gl();
			}
			check_id(id_val);
			break;
		}	
		case LEX_NUM:		
			lex_stack.push(LEX_INT);									//    тип числа - int. Кладем его в стек
			Poliz.push_back(curr_lex);
			gl();
			break;
		
		case LEX_PLUS:
			lvalue = 0;
			gl();
			C();
			check_unary_op();
			break;
		
		case LEX_MINUS:
			lvalue = 0;
			gl();
			C();
			check_unary_op();
			Poliz.push_back(Lexeme(LEX_UNARY_MINUS));
			break;
			
		case LEX_PLUS_PLUS: case LEX_MINUS_MINUS:
			lvalue = 0;
			lexeme_type unary_op_type;
			curr_type == LEX_PLUS_PLUS ? unary_op_type = LEX_PP_PRE : unary_op_type = LEX_MM_PRE;// сохраняем тип унарной операции для добавления в полиз
			
			gl();
			if(curr_type != LEX_ID)
				syn_error("33: Lvalue requied as an increment operand");//#33
			check_id(curr_val);
			check_unary_op();
			if(!pp_id)
			{
				Poliz.push_back(Lexeme(POLIZ_ADDRESS, curr_val));
				Poliz.push_back(Lexeme(LEX_ID, curr_val));
				Poliz.push_back(Lexeme(LEX_NUM, 1));
				unary_op_type == LEX_PP_PRE ? Poliz.push_back(Lexeme(LEX_PLUS)) : Poliz.push_back(Lexeme(LEX_MINUS));
				Poliz.push_back(Lexeme(LEX_ASSIGN));
			}
			else
			{
				Poliz.push_back(Lexeme(POLIZ_ADDRESS, curr_val));
				Poliz.push_back(Lexeme(unary_op_type));
			}
			gl();
			break;
		
		case LEX_QUOTE:
			gl();
			lex_stack.push(LEX_STRING);
			Poliz.push_back(curr_lex);
			if(curr_type != LEX_STR_CONST)
				syn_error("34: No string constant found");				//#34
			gl();														//    считываем завершающую ковычку (при ее отсутствии возникнет лексическая ошибка)
			gl();
			break;
		
		case LEX_TRUE: case LEX_FALSE:
			lex_stack.push(LEX_BOOL);									//    тип true/false - bool. Кладем его в стек
			curr_type == LEX_TRUE ? Poliz.push_back(Lexeme(LEX_TRUE, 1)) : Poliz.push_back(Lexeme(LEX_FALSE, 0));
			gl();
			break;
		
		case LEX_NOT:
			lvalue = 0;
			gl();
			C();
			check_not();
			Poliz.push_back(Lexeme(LEX_NOT));
			break;
		
		case LEX_LEFT_PAREN:
			gl();
			STMNT(0);
			if(curr_type != LEX_RIGHT_PAREN)
				syn_error("35: Did you forget ')' ?");					//#35
			gl();
			break;
		
		default:
			syn_error("36: No matching operand found");					//#36
			break;
	}
}


// СЕМАНТИЧЕСКИЙ АНАЛИЗ
void Parser::dec()														// Настройка типа переменной и проверка ее объявления
{
	if(ID_Table[curr_val].get_declare())								//    если переменная была уже объявлена, то ошибка
		sem_error("Variable \"" + ID_Table[curr_val].get_name() + "\" is declared twice");
	else 																//    иначе
	{
		ID_Table[curr_val].set_type(lex_stack.top());					//		задаем для переменной заданный тип (он находится в конце стека)
		ID_Table[curr_val].set_declare();								//      говорим, что переменная объявлена
	}
}

void Parser::check_id(int val)											// Проверка идентификатора (объявлен или нет)
{
	if(ID_Table[val].get_declare())										//    если объявлен, то
		lex_stack.push(ID_Table[val].get_type());						//      добавляем в стек
	else																//    иначе ошибка
		sem_error("Variable \"" + ID_Table[val].get_name() + "\" has not been declared");
}

void Parser::check_id_in_read()											// Проверка идентификатора (объявлен или нет) в функции read
{
	if(!ID_Table[curr_val].get_declare())
		sem_error("in 'read()' function: Variable \"" + ID_Table[curr_val].get_name() + "\" has not been declared");
}

void Parser::check_op()													// Проверка отдельной операции
{
	lexeme_type o1, o2, op;												//    o1 - левый операнд, o2 - правый операнд, op - оператор
	lexeme_type op_type, res_type;										//    op_type - тип операции, res_type - тип результата операции
	
	extract(lex_stack, o2);
	extract(lex_stack, op);
	extract(lex_stack, o1);
	
	if(o1 == LEX_STRING && o1 == o2)
	{
		op_type = LEX_STRING;
		if(op == LEX_PLUS)
			res_type = LEX_STRING;
		else if(op == LEX_EQ || op == LEX_NOT_EQ ||
				op == LEX_GREATER || op == LEX_LESS)
			res_type = LEX_BOOL;
		else
			sem_error("Unallowed operator for variables of type \"string\"");
	}
	else
	{
		if(op >= LEX_EQ && op <= LEX_NOT_EQ)
		{ op_type = LEX_INT; res_type = LEX_BOOL; }
		
		else if(op >= LEX_PLUS && op <= LEX_PERCENT)
		{ op_type = LEX_INT; res_type = LEX_INT; }
		
		else if(op == LEX_OR || op == LEX_AND)
		{ op_type = LEX_BOOL; res_type = LEX_BOOL; }
	}
	if((o1 == o2 && o1 == op_type) || (op_type == LEX_BOOL && o1 != LEX_STRING && o2 != LEX_STRING))
		lex_stack.push(res_type);
	else
		sem_error("Variable types in the operation do not match");
}

void Parser::check_unary_op()											// Проверка унарной операции
{
	if(lex_stack.top() != LEX_INT)										//    в конце стека находится тип операнда
		sem_error("Wrong type for unary operation");
}

void Parser::check_not()
{
	lexeme_type type = lex_stack.top();
	if(type != LEX_BOOL)
		sem_error("Wrong type in 'not' statement");
}

void Parser::eq_type()													// Проверка схожести типа переменной и выражения при присваивании
{
	lexeme_type o2;														//    o2 - тип выражения (или правой переменной) 
	
	extract(lex_stack, o2);												//    извлекаем тип о2 из стека

	if(lex_stack.top() != o2 && (lex_stack.top() != LEX_BOOL || o2 != LEX_INT))//разрешается присваивать целочисленные выражения логическим переменным
		sem_error("The types do not match");
}

void Parser::eq_bool()													// Проверка типа выражения в условии оператора if/while/for/do-while
{																		// (должен быть логический)
	if(lex_stack.top() == LEX_BOOL)
		lex_stack.pop();
	else
		sem_error("The expression is not boolean");
}

void Parser::break_controller(string mode)
{
	if(mode == "on")
	{ loop = 1; nested_loops++; }
	else if(mode == "off")
	{
		if(!nested_loops)												//    если завершен обычный цикл, а не вложенный, то
			loop = 0;													//      выходим из состояиния цикла
		
		break_stack_elem x;	
		while(!break_stack.empty())										//    если в цикле (обычном или вложенном) были операторы break, то в стеке break_stack сохранились номера цикла, в котором они были вызваны, и номера их меток в полизе
		{																//    поэтому если стек не пуст, то
			extract(break_stack, x);									//      вынимаем очередной элемент стека break - номер цикла, в котором был вызван оператор + номер метки в полизе
			if(x.nl_num == nested_loops)
				Poliz[x.pos] = Lexeme(POLIZ_LABEL, Poliz.size());			//    присваиваем этой метке лексему с координатой перехода в конец цикла
			else
			{ break_stack.push(x); break; }
		}
		nested_loops--;
	}
	else
		sem_error("unknown break_switch() mode\n"); 
}

void Parser::check_break()
{
	if(loop)															//    если находимся в цикле, то
	{
		break_stack_elem x(nested_loops, Poliz.size());					//
		break_stack.push(x);											//      кладем порядковый номер метки break в полизе
		Poliz.push_back(Lexeme());										//      кладем пустую лексему в полиз (позже рисвоим ей координату перехода)
		Poliz.push_back(Lexeme(POLIZ_GO));								//      кладем символ перехода в полиз
	}
	else																//    иначе ошибка
		sem_error("'break' can only be used in cycles");
}

void Parser::check_goto()
{
	vector <ID>::iterator it;
	for(it = ID_Table.begin(); it != ID_Table.end(); ++it)
		if(it->is_label())
		{
			if(it->get_assign() && it->get_address() == -1)
				sem_warning("label \""+ it->get_name() + "\" declared, but not used");
			if(!it->get_assign() &&it->get_address() != -1)
				sem_error("label \""+ it->get_name() + "\" used, but not declared");
		}
}


// ПРЕОБРАЗОВАНИЕ В ПОЛИЗ
void Parser::unary_op_convertion()
{
	int val;
	while(!plus_stack.empty())
	{
		extract(plus_stack, val);
		Poliz.push_back(Lexeme(POLIZ_ADDRESS, val));
		Poliz.push_back(Lexeme(LEX_ID, val));
		Poliz.push_back(Lexeme(LEX_NUM, 1));
		Poliz.push_back(Lexeme(LEX_PLUS));
		Poliz.push_back(Lexeme(LEX_ASSIGN));
	}
	while(!minus_stack.empty())
	{
		extract(minus_stack, val);
		Poliz.push_back(Lexeme(POLIZ_ADDRESS, val));
		Poliz.push_back(Lexeme(LEX_ID, val));
		Poliz.push_back(Lexeme(LEX_NUM, 1));
		Poliz.push_back(Lexeme(LEX_MINUS));
		Poliz.push_back(Lexeme(LEX_ASSIGN));
	}
}


// ВЫПОЛНЕНИЕ ПРОГРАММЫ НА МОДЕЛЬНОМ ЯЗЫКЕ
class Executer															// КЛАСС ИСПОЛНИТЕЛЬ (выполнение команд программы)
{
	Lexeme curr;
    
    stack <int> args;
    stack <string> str_consts;
	stack <lexeme_type> types_stack;
	
	void exec_error(string err_des)
	{
		cerr << "EXECUTION ERROR: " << err_des << endl;
		exit(1);
	}
	
	void exec_warning(string err_des)
	{
		try
		{ throw err_des; }
		catch(string s)
		{ cerr << "WARNING: " << s << endl << endl; }
	}
public:
	void execute(vector <Lexeme> &poliz);
	
	void write()
	{
		if(!types_stack.empty())
		{
			lexeme_type t;
			int x;
			string s;
			extract(types_stack, t);
			switch(t)
			{
				case LEX_STRING:
					extract(str_consts, s);
					break;
				case LEX_INT: case LEX_BOOL:
					extract(args, x);
					break;
				default:
					break;
			}
			write();
			switch(t)
			{
				case LEX_STRING:
					cout << s;
					break;
				case LEX_BOOL:
					x ? cout << "true" : cout << "false";
					break;
				case LEX_INT:
					cout << x;
					break;
				default:
					break;
			}
		}
	}
};

void Executer::execute(vector <Lexeme> &poliz)
{
    int i, j;
    string a, b;
    int index = 0;
	int size = poliz.size();
	
    cout << "Beginning execution...\n\n";
    
    while (index < size)
    {
        curr = poliz[index];
        switch(curr.type())
        {
			case POLIZ_LABEL:
                args.push(curr.val());
                break;
                
			case POLIZ_ADDRESS:
				args.push(curr.val());
				types_stack.push(ID_Table[curr.val()].get_type());
				break;
				
			case LEX_NUM:
				args.push(curr.val());
				types_stack.push(LEX_INT);
				break;
				
			case LEX_TRUE: case LEX_FALSE:
				args.push(curr.val());
				types_stack.push(LEX_BOOL);
				break;
                
			case LEX_STR_CONST:
				str_consts.push(STR_CONST_Table[curr.val()]);
				types_stack.push(LEX_STRING);
				break;
 
            case LEX_ID:
                i = curr.val();
                if(ID_Table[i].get_assign())
                {
					types_stack.push(ID_Table[i].get_type());
					if(ID_Table[i].get_type() == LEX_STRING)
						str_consts.push(ID_Table[i].get_str_value());
					else
						args.push(ID_Table[i].get_value());
				}
                else
					exec_error("the identificator \"" + ID_Table[i].get_name() + "\" doesn't have a value");
				break;
				
            case LEX_NOT:
                extract(args, i);
                args.push(!i);
                break;
 
            case LEX_OR:
                extract(args, i); 
                extract(args, j);
                args.push(j || i);
                types_stack.pop();
                break;
 
            case LEX_AND:
				extract(args, i);
                extract(args, j);
                args.push (j && i);
                types_stack.pop();
                break;

			case LEX_PLUS:
				if(types_stack.top() == LEX_STRING)
				{
					extract(str_consts, a);
					extract(str_consts, b);
					str_consts.push(b + a);
				}
				else
				{
					extract(args, i);
					extract(args, j);
					args.push(j + i);
				}
				types_stack.pop();
				break;
				
			case LEX_MINUS:
				extract(args, i);
				extract(args, j);
				args.push(j - i);
				types_stack.pop();
				break;
 
            case LEX_TIMES:
                extract(args, i);
                extract(args, j);
                args.push(j * i);
                types_stack.pop();
                break;
				
            case LEX_SLASH:
                extract(args, i);
                extract(args, j);
                types_stack.pop();
                if(i)
					args.push(j / i);
				else
					exec_error("dividing by zero is illegal");
				break;
					
			case LEX_PERCENT:
                extract(args, i);
                extract(args, j);
                types_stack.pop();
                if(i)
					args.push(j % i);
				else
					exec_error("dividing by zero is illegal");
				break;
					
			case LEX_UNARY_MINUS:
				extract(args, i);
				args.push(-1 * i);
				break;
				
			case LEX_PP_PRE: case LEX_MM_PRE:
			{	
				extract(args, i);
				int id_val = ID_Table[i].get_value();
//				cout << << endl;
				int op = 1;
				if(curr.type() == LEX_MM_PRE)
					op = -1;
				args.push(id_val + op);
				ID_Table[i].set_value(id_val + op);
				break;
			}	
            case LEX_EQ:
				if(types_stack.top() == LEX_STRING)
				{
					extract(str_consts, a);
					extract(str_consts, b);
					args.push(b == a);
				}
				else
                {
					extract(args, i);
					extract(args, j);
					args.push(j == i);
				}
				types_stack.pop(); types_stack.pop();
				types_stack.push(LEX_BOOL);
                break;
                
			case LEX_NOT_EQ:
                if(types_stack.top() == LEX_STRING)
				{
					extract(str_consts, a);
					extract(str_consts, b);
					args.push(b != a);
				}
				else
                {
					extract(args, i);
					extract(args, j);
					args.push(j != i);
				}
				types_stack.pop(); types_stack.pop();
				types_stack.push(LEX_BOOL);
                break;
 
            case LEX_LESS:
				if(types_stack.top() == LEX_STRING)
				{
					extract(str_consts, a);
					extract(str_consts, b);
					args.push(b < a);
					cout << args.top();
				}
				else
                {
					extract(args, i);
					extract(args, j);
					args.push(j < i);
				}
				types_stack.pop(); types_stack.pop();
				types_stack.push(LEX_BOOL);
                break;
 
            case LEX_GREATER:
                if(types_stack.top() == LEX_STRING)
				{
					extract(str_consts, a);
					extract(str_consts, b);
					args.push(b > a);
				}
				else
                {
					extract(args, i);
					extract(args, j);
					args.push(j > i);
				}
				types_stack.pop(); types_stack.pop();
				types_stack.push(LEX_BOOL);
                break;
 
            case LEX_LESS_EQ:
                extract(args, i);
                extract(args, j);
                args.push(j <= i);
                types_stack.pop(); types_stack.pop();
				types_stack.push(LEX_BOOL);
                break;
 
            case LEX_GREATER_EQ:
                extract(args, i);
                extract(args, j);
                args.push(j >= i);
                types_stack.pop(); types_stack.pop();
				types_stack.push(LEX_BOOL);
                break;
 
            case LEX_ASSIGN:
				types_stack.pop();
				switch(types_stack.top())
				{
					case LEX_STRING:
						extract(str_consts, a);
						extract(args, j);
						ID_Table[j].set_value(a);
						break;
					case LEX_BOOL:
						extract(args, i);
						extract(args, j);
						if(i)
							i = 1;
						ID_Table[j].set_value(i);
						break;
					case LEX_INT:
						extract(args, i);
						extract(args, j);
						ID_Table[j].set_value(i);
						break;
					default:
						break;
				}
				types_stack.pop();
				ID_Table[j].set_assign();
                break;
 
            case POLIZ_GO:
                extract(args, i);
                index = i - 1;
                break;
 
            case POLIZ_FGO:
                extract(args, i);
                extract(args, j);
                types_stack.pop();
                if (!j)
					index = i - 1;
                break;
 
            case LEX_WRITE:
				write();
				break;
				
			case LEX_WRITELINE:
				write();
				cout << endl;
				break;
 
			case LEX_READ:
				int k;
				extract(args, i);
				switch(types_stack.top())
				{
					case LEX_INT:
						cout << "\nEnter int value for " << ID_Table[i].get_name() << ": ";
						cin >> k;
						ID_Table[i].set_value(k);
						break;
					case LEX_STRING:
						cout << "\nEnter string value for " << ID_Table[i].get_name() << ": ";
						cin >> a;
						ID_Table[i].set_value(a);
						break;
					case LEX_BOOL:
						k = 0;
						cout << "\nEnter bool value for " << ID_Table[i].get_name() << ": ";
						cin >> b;
						if(b == "true" || (isdigit(b[0]) && b[0] - '0') || ((b[0] == '+' || b[0] == '-') && isdigit(b[1]) && b[1] - '0'))
							k = 1;
						ID_Table[i].set_value(k);
						break;
					default:
						break;
				}
				types_stack.pop();
				ID_Table[i].set_assign();
				break;

			default:
				exec_error("unknown element");
				break;
		}
		++index;
	}
	cout << "\nExecution complete!\n";
}


class Interpreter
{
	Parser pars;
	Executer exec;

public:
	Interpreter(const string prog_name): pars(prog_name){}
	
	void interpret();
};

void Interpreter::interpret()
{
	pars.analyse();
	exec.execute(pars.Poliz);
}


int main()
{
	string prog_name = "test_program";
	
	cout << "Enter program name: ";
	cin >> prog_name;
	
	Interpreter I(prog_name);
	I.interpret();
	
	return 0;
}

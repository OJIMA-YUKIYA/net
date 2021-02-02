

//
// 東京都立大学 情報科学科
// 助教・柴田祐樹
// ２０１９年１０月　初版
// ２０２０年１０月　改訂
//
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h> // https://linux.die.net/man/2/read
using namespace std;

const int BUFF_SIZE = 128; // 一時記憶の長さ
const int MAX_TABLE_SIZE = 256;

int TABLE_SIZE = 0;

struct Table {
	string msg;
	string from;
	string to;
	time_t time;
};

int main(int argc, char *argv[])
{
    cout << "command server v1.0.0" << endl;

    int port_num = 5000;

    struct sockaddr_in serv_addr, clnt_addr; // ソケットアドレス
    int serv_socket, clnt_socket;            // ソケット記述子
    socklen_t addr_len;                      // アドレス長

    time_t now;             // 時間
    char buff[BUFF_SIZE]; // 受信用一時記憶（６４バイト）
    struct Table chat[MAX_TABLE_SIZE];
    
    // パラメータの初期化
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_num);
    addr_len = sizeof(clnt_addr);

    serv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_socket < 0)
    {
        cout << "Failed to create a socket.\n";
        return -1;
    }
    if (bind(serv_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "Failed to bind a socket to the system.\n";
        return -1;
    }
    if (listen(serv_socket, 5) < 0)
    {
        cout << "Filaed to listen to a socket.\n";
        return -1;
    }
    while (true)
    {
        cout << "Waiting for a client..." << endl;
        clnt_socket = accept(serv_socket, (struct sockaddr *)&clnt_addr, &addr_len);

        cout << "Accepted a connection from [" << inet_ntoa(clnt_addr.sin_addr) << "," << htons(clnt_addr.sin_port) << "]" << endl;
        string operation, name1, name2, msg, fsm;
        string os;
        int state = 0;
        bool escape = false;
        time(&now);
        cout << "state: " << state << ", waiting for messages..\n"; // 現在の状態を確認する
       	int n = read(clnt_socket, buff, BUFF_SIZE - 1);
        if (n <= 0) // 0は相手が接続を遮断したこと，負の値は接続に不具合が生じたことを意味する．正の値は受信に成功したことを意味する．
        {
         	continue;
        }
        buff[n] = 0;
            
        string sign;
        for (int i = 0; i <= 6; i++) {
            sign += buff[i];
        }
       	if (sign != "1SAUYKj") {
        	//初めの文字列が1SAUYKjでないとき、他のアプリケーションプロトコルを受信した可能性があるので、closeする。
        }
        else {
        	cout << "got " << n << " byte, " << buff;
        	for (int i = 7; i < n; ++i) // 受け取った文字列を一つずつ（1Byte毎）に検査する．
        	{
        		//cout << "buff[" << i << "] = " <<  buff[i] << endl;
           		if (buff[i] == '\"') {
            		escape = !escape;
            	}
            	else if (escape) {
              		os += buff[i]; // ""で囲まれた中なので，空白が来たとしても文字列として加算する．
            	}
            	else if (buff[i] == ' ' || buff[i] == '\n') // 空白が来たところで文字列を区切る．改行コードも空白と定義．タブを含めても良い．
            	{
              		if (state == 0) {
                  		operation = os;
                   		if (operation == "get-from") {
                      		state = 1;//次のname1を確認
                        }
                    	else if (operation == "send-to") {
                        	state = 1;//次のname1を確認
                    	}
                    	else {
                        	cout << operation << "is not defined.\n";
                    	}
              		}
                	else if (state == 1) {
                    	name1 = os;
                    	state = 2;//次のname2を確認
                	}
                	else if (state == 2) {
                    	name2 = os;
                    	cout << name2 << endl;
                    	if (operation == "get-from") {
                    		bool flag = true;
                    		if (name2 == "all") {
                    			for (int entry = 0; entry < TABLE_SIZE; entry++) {
                    				if (name1 == chat[entry].to || "all" == chat[entry].to) {
                    					flag = false;
                    					fsm += ctime(&chat[entry].time) + chat[entry].from + "から" + chat[entry].to + "へ\n" + chat[entry].msg + "\n\n";
                    				}
                    			}
                    		}
                    		else {
                    			for (int entry = 0; entry < TABLE_SIZE; entry++) {
                    				if (("all" == chat[entry].to || name1 == chat[entry].to) && name2 == chat[entry].from) {
                    					flag = false;
                    					fsm += ctime(&chat[entry].time) + chat[entry].from + "から" + chat[entry].to + "へ\n" + chat[entry].msg + "\n\n";

                    				}
                    			}
                    		}
                    		if (flag) {
                    			fsm += name2 + "から" + name1 + "へのメッセージはありません\n\n";
                    		}
                    		state = 0;//命令待機状態へ戻る
                    		break;
                    	}
                    	else if (operation == "send-to") {
                    		state = 3;//次のmsgを確認
                    		cout << state << endl;
                    	}
	         		}
                	else if (state == 3) {
                   		msg = os;
                    	chat[TABLE_SIZE].from = name1;
                    	chat[TABLE_SIZE].to = name2;
                    	chat[TABLE_SIZE].msg = msg;
                    	time(&now);
                    	chat[TABLE_SIZE].time = now;
                    	//cout << "from = " << chat[TABLE_SIZE].from << ", to = " << chat[TABLE_SIZE].to << ", msg = " << chat[TABLE_SIZE].msg << endl;
                    	fsm = chat[TABLE_SIZE].from + "様。" + chat[TABLE_SIZE].to + "さんへ以下のメッセージを送信しました。\n" + chat[TABLE_SIZE].msg + "\n";
                    	TABLE_SIZE++;
                    	state = 0; // 命令待機状態へ戻る．
                	}
                	os = "";
                }
            	else {
              		os += buff[i]; // 空白が来るまで文字列へ文字を格納
           		}
           		//cout << "os = " << os << endl;
           	}
           	fsm += string("Time: ") + ctime(&now);
           	fsm.pop_back();
           	fsm += " from 小島の簡易掲示板サーバ\n";
           	write(clnt_socket, fsm.c_str(), fsm.size());
      	}
        close(clnt_socket);
    }
    close(serv_socket);
    return 0;
}

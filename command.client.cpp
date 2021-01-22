//
// 東京都立大学 情報科学科
// 助教・柴田祐樹
// ２０１９年１０月　初版
// ２０２０年１０月　改訂
//

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h> // https://linux.die.net/man/2/read
#include <string>
#include <sstream>


const int BUFF_SIZE = 128; // 一時記憶の長さ

int main(int argc, char *argv[])
{
    using namespace std;
    cout << "command client v1.1.0" << endl; // ソースコードへの変更を行ったら数値を変える．

    // サーバのアドレスとポート番号
    // 127.0.0.1は、ループバックアドレス
    // 他のPCと通信する場合は、当該PCのIPアドレスに変更する。
    string serv_ip = "127.0.0.1";
    in_port_t serv_port = 5000;
    if (argc > 1)
    {
        serv_ip = argv[1];
    }
    if (argc > 2)
    {
        serv_port = stoi(argv[2]);
    }

    // サーバのアドレス等を初期化。
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
    serv_addr.sin_port = htons(serv_port);

    // ソケット作成、入力はIP、ストリーム型、TCPを指定。
    int socketd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketd < 0)
    {
        cout << "Failed to createa socket\n";
        return -1;
    }
    // サーバに接続する。
    int ret = connect(socketd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret < 0)
    {
        cout << "Failed to connect to the server\n";
        return -1;
    }
    // 接続すると、サーバは現在時刻を文字列として返信する。
    // read(.)により、データを受信する。
    string ope;
    char buff[BUFF_SIZE];
    int n; // read の戻り値格納用変数

    
    while (1) {
    	ope = "1SAUYKj";
    	string tmp_ope;
    	getline(cin, tmp_ope);
    	ope += tmp_ope + "\n";
    	write(socketd, ope.c_str(), ope.size());
    	n = read(socketd, buff, BUFF_SIZE - 1);
    	if (n < 0) {
  			cout << "failed to read from a socket\n";
        	return -1;
    	}
    	buff[n] = 0;
    	cout << buff;
    }
    
    close(socketd);
}

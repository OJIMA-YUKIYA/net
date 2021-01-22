//
// 情報通信応用実験 ネットワークプログラミング
//
// 首都大学東京 システムデザイン学部 情報通信システムコース
// 准教授・酒井和哉
// ２０１５年２月５日
//
// 情報科学科
// 助教・柴田祐樹
// ２０１９年１０月　改訂
// ２０２０年１０月　改訂
//

#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <unistd.h> // https://linux.die.net/man/2/read

const int BUFF_SIZE = 1024; // バッファのサイズ


/*
 * UDP Daytimeクライアント
 */
int main(int argc, char* argv[])
{
    using namespace std;
    using namespace chrono;
    cout << "upd time client v1.0.0" << endl; // ソースコードへの変更を行ったら数値を変える．
    char* serv_ip = "127.0.0.1"; // ループバックアドレス
    in_port_t port_num = 5000; // ポート番号
    int n = 0; // 戻り値の保存用

    socklen_t addr_len;
    char buff[BUFF_SIZE]; // 送受信用バッファ

    if (argc == 2) {
        serv_ip = argv[1];
    }
    // パラメータの初期化
    struct sockaddr_in   serv_addr; // アドレス構造体
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip);
    serv_addr.sin_port = htons(port_num);

    // ソケットの作成．UDPを用いるため第２引数にDatagram，第３引数にUDPを指定する．
    int socketd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketd < 0) {
        cout << "Failed to create a client socket.\n";
        return -1;
    }

    cout << inet_ntoa(serv_addr.sin_addr) << " に送るメッセージを作成\n";
    
    // メッセージ送信．
    string msg;
    while (1) {
    	string tmp_msg;
    	getline(cin, tmp_msg);
    	if (tmp_msg == "q" || tmp_msg == "quit" || tmp_msg == "exit") {
    		msg.pop_back();
    		break;
    	}
    	else {
    		msg = msg + tmp_msg + '\n';
    	}
    }

    auto RTT_begin = system_clock::now();//サーバーへ送信する直前の時刻を取得
    n = sendto(socketd, msg.c_str(), msg.size(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (n < 0) {
        cout << "failed to receive a message.\n";
        return -1;
    }

    
    // サーバから現在時刻を文字列として受信．
    addr_len = sizeof(serv_addr);
    n = recvfrom(socketd, buff, sizeof(buff)-1, 0, (struct sockaddr*)&serv_addr, &addr_len); // 終端文字列を入れるために，sizeof(buff)-1 として，文字列一つ分必ず余裕を持たせてデータを受信する．buff をこのまま文字列として使わない場合は全記憶を受信に使う．
	auto RTT_end = system_clock::now();//サーバーから受信した直後の時刻を取得

	
	duration<double> RTT_time = RTT_end - RTT_begin;
    if (n < 0) {
        cout << "Failed to receive a message.\n";
        return -1;
    }
    buff[n] = 0; // 終端文字列を追加．送信者が終端文字列を入れてデータを送ってきているとは限らない．
    cout << "Time: " << buff;
    cout << "Round Trip Time: "<<  RTT_time.count() << "秒\n";

    // ソケットを閉じる
    close(socketd);
}

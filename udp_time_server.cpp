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

const int BUFF_SIZE = 1048576; // バッファのサイズ

/*
 * UDP Daytimeサーバ.
 *
 */
int main(int argc, char* argv[])
{
    using namespace std;
    cout << "upd time server v1.0.0" << endl; // ソースコードへの変更を行ったら数値を変える．

    // パラメータ
    int port_num = 5000; // 待ち受けポート番号
    struct sockaddr_in   serv_addr, clnt_addr; // サーバとクライアントのソケットアドレス
    int serv_socket; // ソケット記述子
    socklen_t addr_len; // アドレス長
    int n = 0; // 戻り値の保存用

    char buff[BUFF_SIZE]; // 送信用バッファ
    time_t now; // 現在時刻の保存用変数

    // パラーメータ初期化
    serv_addr.sin_family = AF_INET; // IPv4 プロトコルファミリー
    serv_addr.sin_addr.s_addr = INADDR_ANY; // インターネットアドレス
    serv_addr.sin_port = htons(port_num); // ポート番号設定

    // ソケット作成
    // 引数にIPv4, データグラム，UDPを指定する．
    serv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serv_socket < 0){
        cout << "ソケット作成に失敗しました.\n";
    }
    // バインド（ソケットとポートの結合）
    if (bind(serv_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "ソケットとポートの結合に失敗したため異常終了しました.\n";
        return -1;
    }

    // クライアントからのクエリを待ち受け．
    while (true) {
        // クライアントからクエリ文字列を待ち受ける．
        // UDPはコネクションを確立しないため，クライアントがクエリ文字列を送ってくるのを待機．
        cout << "クライアントからのメッセージを待っています...\n";
        addr_len = sizeof(clnt_addr);
        n = recvfrom(serv_socket, buff, BUFF_SIZE, 0, (struct sockaddr*)&clnt_addr, &addr_len);
        if (n < 0) {
            cout << "failed to read a query from the socket.\n";
            return -1;
        }

        cout << "[IP アドレス: " << inet_ntoa(clnt_addr.sin_addr) << ", ポート番号: " << htons(clnt_addr.sin_port) << "] から以下のメッセージを受信しました." << endl;
        if (n == BUFF_SIZE) {
        	cout << "Warning!!: 送信された文字数が多いため途中までしか表示されません" << endl;
        }
        buff[n] = '\0';//終端文字列を追加　必ずしも送信者が終端文字を入れているとは限らない。
		cout << buff << "\n";

        // 現在時刻取得
        time(&now);
        //クライアントに送信するメッセージmsgを作成
        string msg = ctime(&now); // string クラスは加算演算子で文字列を結合可能．
        msg.pop_back();
        msg += string(" from 小島のUDPタイムサーバー\n");

        // 現在時刻を文字列として，クライアントに送信する．
        n = sendto(serv_socket, msg.c_str(), msg.size(), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
        if (n < 0) {
            cout << "Failed to write a message to the socket.\n";
            return -1;
        }
    }

    // ソケットを閉じる．
    close(serv_socket);
    return 0;
}

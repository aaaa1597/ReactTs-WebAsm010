# ReactTs-WebAsm010
React+TypeScriptでWebAssembly010。Wasm。js->C++の関数コールを調べてみた。

解説ページ : https://zenn.dev/rg687076/articles/2d640f95404b4a

![](https://storage.googleapis.com/zenn-user-upload/6d8967181385-20240130.png)

見た目地味だけど、盛だくさん。

下記の関数呼び出しのサンプルを実装。

1. int,int,int
2. double,double,double
3. std::string, std::string, std::string
4. int配列, int配列, int配列
5. UInt8配列, UInt8配列, UInt8配列
6. string配列, string配列, string配列
7. JSオブジェクト, JSオブジェクト, JSオブジェクト
8. JSONオブジェクト, JSONオブジェクト, JSONオブジェクト

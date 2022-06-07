package server_part;

import java.net.ServerSocket;
import java.net.Socket;

public class Server {
    public static void main(String[] args) throws Exception {
        int port = 8080;
        ServerSocket server = new ServerSocket(port);
        Socket client;
        System.out.println ("Server started, port:" + port);
        System.out.println();
        while (true) {
            client = server.accept();
            System.out.println (client.getPort() + " connected to the server");
            Handler handler = new Handler(server,client);
            new Thread(handler).start();
        }
    }
}
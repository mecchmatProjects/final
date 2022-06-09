package server_part;

import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;

public class ServerHttpManager {
    ServerSocket server;
    Socket client;
    public ServerHttpManager(ServerSocket server, Socket client){
        this.client = client;
        this.server = server;
    }
    public String OK(String responseType,String reply,String username){
        String massage = "HTTP/1.0 200 OK\r\n" +
                "Server: localhost"+ server.getLocalPort() +"\r\n" +
                "Content-Type: text\r\n" +
                "Response-Type: " + responseType + "\r\n" +
                "Connection: keep-alive\r\n" +
                "Reply= " + reply + "\r\n" +
                "Username= " + username + "\r\n\r\n";
        return massage;
    }
    public String OK(String responseType, String reply, ArrayList<String> roots){
        int size = roots.size();
        String massage = "HTTP/1.0 200 OK\r\n" +
                "Server: localhost"+ server.getLocalPort() +"\r\n" +
                "Content-Type: text\r\n" +
                "Response-Type: " + responseType + "\r\n" +
                "Connection: keep-alive\r\n" +
                "Reply= " + reply + "\r\n" +
                "Size= " + size + "\r\n";
        for (int i = 0; i < size; i++){
            massage = massage + "x" + i +"= "+ roots.get(i)+ "\r\n";
        }
        massage = massage + "\r\n";
        return massage;
    }
    public String NotFound(String responseType,String reply){
        String massage = "HTTP/1.0 404 Not Found\r\n" +
                "Server: localhost"+ server.getLocalPort() +"\r\n" +
                "Content-Type: text\r\n" +
                "Response-Type: " + responseType + "\r\n" +
                "Connection: keep-alive\r\n" +
                "Reply= " + reply + "\r\n\r\n";
        return massage;
    }
}

package client_part;

import java.math.BigDecimal;
import java.net.Socket;
import java.util.List;

public class ClientHttpManager {
    Socket server;

    public ClientHttpManager(Socket server) {
        this.server = server;
    }

    public String POST(String action, String acceptType, String contentType,String user){
        String massage = "POST / HTTP/1.0\r\n" +
                "Host: localhost" + server.getPort() + "\r\n" +
                "User-Agent: JavaClient\r\n" +
                "Accepted-type: " + acceptType + "\r\n" +
                "Content-type: " + contentType + ";boundary=\"--boundary\"\r\n" +
                "Action: " + action + "\r\n" +
                "Connection: keep-alive\r\n"+
                "User= " + user + "\r\n\r\n";
        return massage;
    }

    public String POST(String action, String acceptType, String contentType, List<BigDecimal> coefs){
        int p_degree = coefs.size();
        String massage = "POST / HTTP/1.0\r\n" +
                "Host: localhost:" + server.getPort() + "\r\n" +
                "User-Agent: JavaClient\r\n" +
                "Accepted-type: " + acceptType + "\r\n" +
                "Content-type: " + contentType + ";boundary=\"--boundary\"\r\n" +
                "Action: " + action + "\r\n" +
                "Connection: keep-alive\r\n"+
                "Size= " + p_degree + "\r\n";
        for (int i = 0; i < p_degree; i++){
            massage = massage + "a" + (p_degree-i) + "= " + coefs.get(i)+ "\r\n";
        }
        massage = massage + "\r\n";
        return massage;
    }

    public String POST(String action, String acceptType, String contentType){
        String massage = "POST / HTTP/1.0\r\n" +
                "Host: localhost" + server.getPort() + "\r\n" +
                "User-Agent: JavaClient\r\n" +
                "Accepted-type: " + acceptType + "\r\n" +
                "Content-type: " + contentType + ";boundary=\"--boundary\"\r\n" +
                "Action: " + action + "\r\n" +
                "Connection: keep-alive\r\n\r\n";
        return massage;
    }

    public String GET(String action, String acceptType, String contentType){
        String massage = "POST / HTTP/1.0\r\n" +
                "Host: localhost" + server.getPort() + "\r\n" +
                "User-Agent: JavaClient\r\n" +
                "Accepted-type: " + acceptType + "\r\n" +
                "Content-type: " + contentType + ";boundary=\"--boundary\"\r\n" +
                "Action: " + action + "\r\n" +
                "Connection: keep-alive\r\n\r\n";
        return massage;
    }
}


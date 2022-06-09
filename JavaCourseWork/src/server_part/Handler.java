package server_part;

import org.json.simple.JSONObject;
import java.io.*;
import java.math.BigDecimal;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import static server_part.Functions.*;

public class Handler implements Runnable{

    private final Socket client;
    private final ServerSocket server;
    String serverAccepted,clientAccept,clientContent;
    String user = "";

    public Handler(ServerSocket server, Socket client){
        this.client = client;
        this.server = server;
    }

    public void run(){
        try{

            BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
            BufferedWriter out = new BufferedWriter(new OutputStreamWriter(client.getOutputStream()));

            ObjectOutputStream outObj = new ObjectOutputStream(client.getOutputStream());
            ObjectInputStream inObj = new ObjectInputStream(client.getInputStream());

            ServerHttpManager httpManager = new ServerHttpManager(server,client);


            String inputStr;
            String outputStr;

            ArrayList<String> massage = new ArrayList<String>();
            Map<String, String> request;
            String requestType,action;

            List<BigDecimal> polinom = new ArrayList<>();
            ArrayList<String> rootsList = new ArrayList<>();
            int p_degree;

            while (true) {
                System.out.println("--------------------------------------");
                while (true) {
                    inputStr = in.readLine();
                    System.out.println(inputStr);
                    if (inputStr.equals("")) break;
                    massage.add(inputStr);
                }
                request = readHttp(massage);
                action = request.get("Action");

                if (!action.equals("login")) {
                    outputStr = httpManager.NotFound("login","Failed to login");
                    out.write(outputStr);
                    out.flush();
                }
                else {
                    clientAccept = request.get("Accepted-type");
                    clientContent = request.get("Content-type");
                    user = request.get("User");
                    if (!user.equals("")){
                        outputStr = httpManager.OK("login","Success",user);
                        out.write(outputStr);
                        out.flush();
                        break;
                    }
                    else{
                        outputStr = httpManager.NotFound("login","Name is empty");
                        out.write(outputStr);
                        out.flush();
                    }

                }
            }
            while (true) {
                System.out.println("--------------------------------------");
                massage.clear();
                while (true) {
                    inputStr = in.readLine();
                    System.out.println(inputStr);
                    if (inputStr.equals("")) break;
                    massage.add(inputStr);
                }
                request = readHttp(massage);
                action = request.get("Action");

                if (action.equals("getRoots")) {
                    p_degree = Integer.parseInt(request.get("Size"));
                    polinom.clear();
                    for(int i = 0; i<p_degree;i++){
                        polinom.add(new BigDecimal(request.get("a"+(p_degree-i))));
                    }
                    System.out.println("receiveed coefs: " + prt(polinom));
                    rootsList = graeffe(polinom);
                    System.out.println("roots: "+prt(rootsList));
                    outputStr = httpManager.OK("Roots","Success",rootsList);
                    out.write(outputStr);
                    out.flush();
                }
                massage.clear();
                while (true) {
                    inputStr = in.readLine();
                    System.out.println(inputStr);
                    if (inputStr.equals("")) break;
                    massage.add(inputStr);
                }
                request = readHttp(massage);
                action = request.get("Action");
                if (action.equals("getJson")){
                    if (!(rootsList.size() ==0)){
                        JSONObject jsonObj = new JSONObject();
                        jsonObj.put("roots", rootsList);
                        outObj.writeObject(jsonObj);
                        outObj.flush();
                    }
                    else {
                        outputStr = httpManager.NotFound("getJson","Roots list is empty");
                        out.write(outputStr);
                    }
                }
            }
        }
        catch (Exception e) {
            System.out.println ("Server error: " + e);
        }
    }
}

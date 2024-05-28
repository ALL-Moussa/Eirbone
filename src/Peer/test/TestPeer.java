package test;

import project.Peer;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class TestPeer {
    public static void main(String[] args) {
        // Créer une instance de Peer avec un fichier de configuration et des chemins de fichiers
        String configFile = "src/Peer/config.ini";
        String configFile2 = "src/Peer/config2.ini";
        String[] filesPaths = {"src/Peer/file1.txt", "src/Peer/file2.txt"};
        String[] filesPaths2 = {"src/Peer/file3.txt"};
        Peer peer = new Peer(configFile, filesPaths);
        Peer peer2 = new Peer(configFile2, filesPaths2);

        // Tester la méthode announce
        peer.Announce();
        peer2.Announce();
        // // Tester la méthode look
        String[] criteria = {"filename=\"file3.txt\""};
        peer.Look(criteria);
        // Tester la methode getFile
        String key = "04e5d10235f01d6031bccbe314eab863";
        peer.getFile(key);
        // Attendre une entrée utilisateur avant de continuer
        System.out.println("Appuyez sur Entrée pour continuer...");
        try {
            System.in.read();
        } catch (IOException e) {
            e.printStackTrace();
        }


    }
}
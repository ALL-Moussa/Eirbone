package project;

public class testParser {
    public static void main(String[] args) {
        System.out.println("Testing Parser..");

        MessageParser m = new MessageParser("> peers 8905e92afeb80fc7722ec89eb0bf0966 [1.1.1.2:2222 1.1.1.3:3333]");
        ParsedMessage p = m.readGetFile();
        assert "8905e92afeb80fc7722ec89eb0bf0966".equals(p.getParsedKey()) : "Erreur de lecture de clé";
        String[] info = p.getParsedInfo();
        assert "1.1.1.2:2222".equals(info[0]) : "Erreur de lecture des adresses";
        assert "1.1.1.3:3333".equals(info[1]) : "Erreur de lecture des adresses";
        System.out.println("Peers parser OK!");

        m = new MessageParser("> have 8905e92afeb80fc7722ec89eb0bf0966 010100011110");
        p = m.readHave();
        assert "8905e92afeb80fc7722ec89eb0bf0966".equals(p.getParsedKey()) : "Erreur de lecture de clé";
        info = p.getParsedInfo();
        assert "010100011110".equals(info[0]) : "Erreur de lecture de buffer map";
        System.out.println("Have parser OK!");

        m = new MessageParser("> data 8905e92afeb80fc7722ec89eb0bf0966 [3:%piece3% 5:%piece5%]");
        p = m.readGetPieces();
        assert "8905e92afeb80fc7722ec89eb0bf0966".equals(p.getParsedKey()) : "Erreur de lecture de clé";
        info = p.getParsedInfo();
        assert "3:%piece3%".equals(info[0]) : "Erreur de lecture de la piece 3";
        assert "5:%piece5%".equals(info[1]) : "Erreur de lecture de la piece 5";
        System.out.println("Piece parser OK!");

        m = new MessageParser("> list [file_a.dat 2097152 1024 8905e92afeb80fc7722ec89eb0bf0966]");
        p = m.readList();
        assert "".equals(p.getParsedKey()) : "Erreur de lecture de clé vide";
        info = p.getParsedInfo();
        assert "file_a.dat".equals(info[0]) : "Erreur de lecture ";
        assert "2097152".equals(info[1]) : "Erreur de lecture";
        assert "1024".equals(info[2]) : "Erreur de lecture";
        assert "8905e92afeb80fc7722ec89eb0bf0966".equals(info[3]) : "Erreur de lecture";
        System.out.println("List parser OK!");

        
    }
}

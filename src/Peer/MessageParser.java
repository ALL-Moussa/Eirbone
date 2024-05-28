package project;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class MessageParser {
    private String message;
    public MessageParser(String Message){
        message = Message;
    }

    // Reads a message in this format: peers 8905e92afeb80fc7722ec89eb0bf0966 [1.1.1.2:2222 1.1.1.3:3333]
    // Which is a response of: getfile 8905e92afeb80fc7722ec89eb0bf0966
    // Returns a Parsed array instance that contains the key: 8905e92afeb80fc7722ec89eb0bf0966 and the array [1.1.1.2:2222 1.1.1.3:3333]
    public ParsedMessage readGetFile(){
        String regex = "peers (\\w+) \\[(.*?)\\]";
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(message);

        if (matcher.find()) {
            String key = matcher.group(1);
            String peersString = matcher.group(2);
            String[] peersArray = peersString.split("\\s+");

            for (int i = 0; i < peersArray.length; i++) {
                // Remove any surrounding brackets if present
                peersArray[i] = peersArray[i].replaceAll("[\\[\\]]", "");
            }

            return new ParsedMessage(key, peersArray);
        }

        return null;
    }

    // Reads a message in this format: have 8905e92afeb80fc7722ec89eb0bf0966 %buffermap%
    // Which is a response of: interested 8905e92afeb80fc7722ec89eb0bf0966 or have 8905e92afeb80fc7722ec89eb0bf0966 %buffermap%
    // Returns a Parsed array instance that contains the key: 8905e92afeb80fc7722ec89eb0bf0966 and the array %buffermap%
    public ParsedMessage readHave(){
        String regex = "have (\\w+) (\\w+)";
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(message);

        if (matcher.find()) {
            String key = matcher.group(1);
            String bufferMap = matcher.group(2);
            String[] buffArr = {bufferMap};

            return new ParsedMessage(key, buffArr);
        }

        return null;
    }

    // Reads a message in this format: data 8905e92afeb80fc7722ec89eb0bf0966 [3:%piece3% 5:%piece5% 7:%piece7% 8:%piece8% 9:%piece9%]
    // Which is a response of: getpieces 8905e92afeb80fc7722ec89eb0bf0966 [3 5 7 8 9]
    // Returns a Parsed array instance that contains the key: 8905e92afeb80fc7722ec89eb0bf0966 and the array [3:%piece3% 5:%piece5% 7:%piece7% 8:%piece8% 9:%piece9%]
    public ParsedMessage readGetPieces(){
        String regex = "> data (\\w+) \\[(.*?)\\]";
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(message);

        if (matcher.find()) {
            String key = matcher.group(1);
            String peersString = matcher.group(2);
            String[] peersArray = peersString.split("\\s+");

            for (int i = 0; i < peersArray.length; i++) {
                // Remove any surrounding brackets if present
                peersArray[i] = peersArray[i].replaceAll("[\\[\\]]", "");
            }

            return new ParsedMessage(key, peersArray);
        }

        return null;
    }

    // Reads a message in this format: list [file_a.dat 2097152 1024 8905e92afeb80fc7722ec89eb0bf0966]
    // Which is a response of: look [filename=”file_a.dat” filesize>”1048576”]
    // Returns a Parsed array instance that contains the array [file_a.dat 2097152 1024 8905e92afeb80fc7722ec89eb0bf0966]
    public ParsedMessage readList(){
        String regex = "> list \\[(.*?)\\]";
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(message);

        if (matcher.find()) {
            String peersString = matcher.group(1);
            String[] peersArray = peersString.split("\\s+");

            for (int i = 0; i < peersArray.length; i++) {
                // Remove any surrounding brackets if present
                peersArray[i] = peersArray[i].replaceAll("[\\[\\]]", "");
            }

            return new ParsedMessage("", peersArray);
        }

        return null;
    }


}

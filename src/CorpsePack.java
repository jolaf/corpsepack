import java.io.*;
import java.util.*;

abstract class Entity  {
    protected Entity(String propertyName) {
    }
}

abstract class Event {
}

class Death extends Event {
}

class Damage extends Event {
    int amount;
    protected Damage(int amount) {
        this.amount = amount;
    }
}

abstract class Handler extends Entity {
    protected Handler(String propertyName) {
        super(propertyName);
    }

    public void handle(Event event) {
    }
}

class ConfigurationException extends Exception {
    public ConfigurationException(String message, Throwable cause)  {
        super(message, cause);
    }

    public ConfigurationException(String message)  {
        super(message);
    }
}

class Configuration extends HashMap {

    static class Link {
        String propertyName;

        Link(String propertyName) {
            this.propertyName = propertyName;
        }
    }

    public Configuration(String fileName) throws ConfigurationException, IOException {
        this(new BufferedReader(new InputStreamReader(new FileInputStream(fileName)))); // ToDo: charset?
    }

    public Configuration(Reader reader) throws ConfigurationException, IOException {
        super(loadProperties(reader));
        String className = (String) get("");
        if (!className.equals(getClass().getName())) {
            throw new ConfigurationException("Configuration class name mismatch: " + className);
        }
        parseProperties();
        init();
        link();
    }

    protected static Properties loadProperties(Reader reader) throws IOException {
        Properties properties = new Properties();
        properties.load(reader);
        return properties;
    }

    protected void parseProperties() throws ConfigurationException {
        // Copy the set to allow in-progress modifications.
        for (Iterator iterator = new HashSet(entrySet()).iterator(); iterator.hasNext(); ) {
            Map.Entry entry = (Map.Entry) iterator.next();
            String key = ((String) entry.getKey()).trim(); // ToDo: check type?
            String value = ((String) entry.getValue()).trim(); // ToDo: check type?
            put(key, parseString(value, key));
        }
    }

    protected Object parseString(Object object, String propertyName) throws ConfigurationException {
        // ToDo: rewrite to handle braces, quotes and commas correctly
        if (object != null) {
            if (object instanceof String) {
                String string = ((String) object).trim();
                if (string.contains(",")) {
                    List list = Arrays.asList(string.split(","));
                    for (int i = 0; i < list.size(); i++) {
                        object = parseString(((String) list.get(i)).trim(), null);
                        //if (object instanceof String) {
                        //    list.set(i, new Link((String) object));
                        //} else {
                            list.set(i, object);
                        //}
                    }
                    return list;
                } else if (string.startsWith("class ")) {
                    if (propertyName != null) {
                        String className = this.getClass().getName() + '$' + string.substring(6).trim();
                        try {
                            Class cls = ClassLoader.getSystemClassLoader().loadClass(className);
                            if (Entity.class.isAssignableFrom(cls)) {
                                return cls.getConstructor(new Class[] { this.getClass(), String.class }).newInstance(new Object[] { this, propertyName });
                            } else {
                                throw new Exception("Referenced class must be a subclass of " + Entity.class.getName());
                            }
                        } catch (Exception e) {
                            throw new ConfigurationException("Error loading class " + className, e);
                        }
                    } else {
                        throw new ConfigurationException("Classes must be referenced directly");
                    }
                }
            }
        }
        return object;
    }

    protected void init() {
        Object handlers = get("handlers"); // ToDo: Check for null
    }

    protected void link() {
    }
}

class Character extends Configuration {
    boolean alive = true;
    int hitPoints = 3; // ToDo
    int hitPointsMin= -1; // ToDo
    int hitPointsDie = -1; // ToDo
    Handler[] handlers = { new Life(""), new HitPoints("") }; // ToDo

    public class Life extends Handler {
        Character character = Character.this;

        public Life(String propertyName) {
            super(propertyName);
        }

        public void handle(Event event) {
            if (event instanceof Death) {
                character.alive = false;
            }
        }
    }

    public class HitPoints extends Handler {
        Character character = Character.this;

        public HitPoints(String propertyName) {
            super(propertyName);
        }

        public void handle(Event event) {
            if (event instanceof Damage) {
                Damage damage = (Damage) event;
                character.hitPoints -= damage.amount;
                if (character.hitPoints < character.hitPointsMin) {
                    character.hitPoints = character.hitPointsMin;
                }
                if (character.hitPoints <= character.hitPointsDie) {
                    character.event(new Death());
                }
            }
        }
    }

    public Character(String fileName) throws ConfigurationException, IOException {
        super(fileName);
    }

    public Character(Reader reader) throws ConfigurationException, IOException {
        super(reader);
    }

    public void event(Event event) {
        for (int i = 0; i < handlers.length; i++) {
            handlers[i].handle(event);
        }
    }
}

public class CorpsePack {
    public static void main(String[] args) throws Exception {
        System.out.println(new Character("test.properties"));
        System.out.println("DONE");
    }
}

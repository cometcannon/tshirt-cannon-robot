package manager;

import java.util.Observable;

public class ObservableCommand extends Observable
{
    private String commandType;
    private int commandMagnitude;

    public ObservableCommand(String type, int magnitude)
    {
        commandType = type;
        commandMagnitude = magnitude;
    }

    public void setValue(String type, int magnitude)
    {
        commandType = type;
        commandMagnitude = magnitude;
        setChanged();
        notifyObservers();
    }

    public int getMagnitude()
    {
        return commandMagnitude;
    }

    public String getType()
    {
        return commandType;
    }
}

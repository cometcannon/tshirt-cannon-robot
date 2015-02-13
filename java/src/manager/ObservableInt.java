package manager;

import java.util.Observable;

public class ObservableInt extends Observable
{
    private int value;

    public ObservableInt(int value)
    {
        this.value = value;
    }

    public void setValue(int value)
    {
        this.value = value;
        setChanged();
        notifyObservers();
    }

    public int getValue()
    {
        return value;
    }
}

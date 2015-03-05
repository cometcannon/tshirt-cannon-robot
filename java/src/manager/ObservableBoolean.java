package manager;

import java.util.Observable;

public class ObservableBoolean extends Observable
{
    private boolean bool;

    public ObservableBoolean(boolean bool)
    {
        this.bool = bool;
    }

    public void setValue(boolean bool)
    {
        this.bool = bool;
        setChanged();
        notifyObservers();
    }

    public boolean getValue()
    {
        return bool;
    }
}

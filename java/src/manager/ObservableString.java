package manager;

import java.util.Observable;

public class ObservableString extends Observable
{
    private String str;

    public ObservableString(String str)
    {
        this.str = str;
    }

    public void setValue(String str)
    {
        this.str = str;
        setChanged();
        notifyObservers();
    }

    public String getValue()
    {
        return str;
    }
}

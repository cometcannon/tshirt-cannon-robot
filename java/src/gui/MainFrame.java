package gui;

import javax.swing.JFrame;

import manager.UIState;

public class MainFrame extends JFrame
{
    UIState uiState;
    
    public MainFrame(UIState uiState)
    {
        this.uiState = uiState;

        DebugPanel debug = new DebugPanel(uiState);
        add(debug);
        
        setSize(400, 250);
        setLocationRelativeTo(null);
        setVisible(true);
    }
}

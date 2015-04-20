package edu.utdallas.cometcannon.gui;

import javax.swing.*;
import java.awt.*;

public class PressurePanel extends JPanel
{
    public PressurePanel()
    {
        setLayout(new BorderLayout());
        JLabel panelTitle = new JLabel("Pressure", SwingConstants.CENTER);
        add(panelTitle, BorderLayout.NORTH);
    }
}

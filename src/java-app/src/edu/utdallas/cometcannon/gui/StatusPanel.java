package edu.utdallas.cometcannon.gui;

import javax.swing.*;

public class StatusPanel extends JPanel
{
    private JLabel statusLabel;
    public StatusPanel()
    {
        statusLabel = new JLabel("<html>Status: <font color='red'>CATASTROPHIC FAILURE! RUN FOR YOUR LIFE!</font></html>");
        add(statusLabel);
    }

    public void updateStatus(String status)
    {
        statusLabel.setText(status);
    }
}

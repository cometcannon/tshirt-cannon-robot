package gui;

import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JComboBox;
import javax.swing.JButton;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import manager.UIState;

public class DebugPanel extends JPanel
{
    UIState uiState;

    JSlider slider;
    JComboBox comboBox;
    JButton sendButton;
    JButton killButton;
    
    protected DebugPanel(UIState uiState)
    {
        this.uiState = uiState;

        slider = new MagnitudeSlider();
        comboBox = new CommandComboBox();
        sendButton = new JButton("SEND");
        killButton = new JButton("KILL");

        sendButton.addActionListener(new SendListener());
        killButton.addActionListener(new KillListener());

        add(slider);
        add(comboBox);
        add(sendButton);
        add(killButton);
    }

    class SendListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            String commandType = (String) comboBox.getSelectedItem();
            int magnitude = slider.getValue();

            uiState.addCommand(commandType, magnitude);
        }
    }

    class KillListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            uiState.addCommand("KILL");
        }
    }
}

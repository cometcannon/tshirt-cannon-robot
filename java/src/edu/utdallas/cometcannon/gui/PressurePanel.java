package edu.utdallas.cometcannon.gui;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;

public class PressurePanel extends JPanel
{
    public PressurePanel()
    {
        setLayout(new BorderLayout());

        JLabel panelTitle = new JLabel("Pressure", SwingConstants.CENTER);
        add(panelTitle, BorderLayout.NORTH);

        VisualPressurePanel visPanel = new VisualPressurePanel();
        add(visPanel, BorderLayout.CENTER);
    }

    class VisualPressurePanel extends JPanel
    {
        VisualPressurePanel()
        {
            final int BORDER_HEIGHT = (int) (MainFrame.HEIGHT * 0.08);
            Border emptyBorder = BorderFactory.createEmptyBorder(BORDER_HEIGHT,
                                                                 BORDER_HEIGHT,
                                                                 BORDER_HEIGHT,
                                                                 BORDER_HEIGHT);
            setBorder(emptyBorder);

            setLayout(new BorderLayout());

            JSlider slider = new JSlider(SwingConstants.VERTICAL, 0, 200, 50);
            slider.setEnabled(false);
            slider.setMajorTickSpacing(50);
            slider.setMinorTickSpacing(10);
            slider.setPaintTicks(true);
            slider.setPaintLabels(true);
            add(slider, BorderLayout.CENTER);
        }
    }
}

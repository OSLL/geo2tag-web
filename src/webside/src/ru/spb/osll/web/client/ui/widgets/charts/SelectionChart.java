/*
 * Copyright 2011-2012 OSLL
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * The advertising clause requiring mention in adverts must never be included.
 */

/*! ---------------------------------------------------------------
 * PROJ: OSLL/geo2tag
 * ---------------------------------------------------------------- */

package ru.spb.osll.web.client.ui.widgets.charts;

import com.google.gwt.core.client.JsArray;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.visualization.client.Selectable;
import com.google.gwt.visualization.client.Selection;
import com.google.gwt.visualization.client.events.SelectHandler;

/**
 * Demo for SelectHandler that can be applied to any Selectable visualization.
 */
class SelectionChart extends SelectHandler {
  private final Selectable viz;
  private final Label label;

  SelectionChart(Selectable viz, Label label) {
    this.viz = viz;
    this.label = label;
  }

  @Override
  public void onSelect(SelectEvent event) {
    StringBuffer b = new StringBuffer();
    JsArray<Selection> s = getSelections();
    for (int i = 0; i < s.length(); ++i) {
      if (s.get(i).isCell()) {
        b.append(" cell ");
        b.append(s.get(i).getRow());
        b.append(":");
        b.append(s.get(i).getColumn());
      } else if (s.get(i).isRow()) {
        b.append(" row ");
        b.append(s.get(i).getRow());
      } else {
        b.append(" column ");
        b.append(s.get(i).getColumn());
      }
    }
    label.setText("selection changed " + b.toString()); 
  }

  private JsArray<Selection> getSelections() {
    return viz.getSelections();
  }
}
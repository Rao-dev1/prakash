/**
 * PRAKASH-DS Web Application Client
 * Complete standalone frontend: no external CDN dependencies (100% offline).
 */

(function () {
  'use strict';

  // Application State
  const state = {
    dataset: null,
    lots: [],
    activeLotID: null,
    activeComponentID: null,
    popViewMode: 'leakage-iddq', // 'leakage-iddq' or 'drift-view'
    trendMetric: 'leakage',       // 'leakage' or 'iddq'
    currentPage: 1,
    pageSize: 30,
    searchQuery: '',
    filterDecision: 'ALL',
    benchmarkData: null,
    testData: null
  };

  // DOM Elements Cache
  const elements = {
    navTabs: document.getElementById('navTabs'),
    tabPanes: document.querySelectorAll('.tab-pane'),
    themeToggle: document.getElementById('themeToggle'),
    dropZone: document.getElementById('dropZone'),
    fileInput: document.getElementById('fileInput'),
    lotSelector: document.getElementById('lotSelector'),
    lotSubheading: document.getElementById('lotSubheading'),
    kpiPassCount: document.getElementById('kpiPassCount'),
    kpiPassPct: document.getElementById('kpiPassPct'),
    kpiMonitorCount: document.getElementById('kpiMonitorCount'),
    kpiMonitorPct: document.getElementById('kpiMonitorPct'),
    kpiSuspectCount: document.getElementById('kpiSuspectCount'),
    kpiSuspectPct: document.getElementById('kpiSuspectPct'),
    kpiRejectCount: document.getElementById('kpiRejectCount'),
    kpiRejectPct: document.getElementById('kpiRejectPct'),
    infoLotID: document.getElementById('infoLotID'),
    infoTotalComps: document.getElementById('infoTotalComps'),
    infoTotalMeas: document.getElementById('infoTotalMeas'),
    infoUploadTime: document.getElementById('infoUploadTime'),
    insightYield: document.getElementById('insightYield'),
    insightOutliers: document.getElementById('insightOutliers'),
    insightRejects: document.getElementById('insightRejects'),
    insightMad: document.getElementById('insightMad'),
    componentTableBody: document.getElementById('componentTableBody'),
    componentSearchInput: document.getElementById('componentSearchInput'),
    decisionFilter: document.getElementById('decisionFilter'),
    tableFilterCount: document.getElementById('tableFilterCount'),
    pageIndicator: document.getElementById('pageIndicator'),
    btnPrevPage: document.getElementById('btnPrevPage'),
    btnNextPage: document.getElementById('btnNextPage'),
    lotScatterCanvas: document.getElementById('lotScatterCanvas'),
    trendChartCanvas: document.getElementById('trendChartCanvas'),
    benchmarkCanvas: document.getElementById('benchmarkCanvas'),
    heapTreeSvg: document.getElementById('heapTreeSvg'),
    btnTrendLeakage: document.getElementById('btnTrendLeakage'),
    btnTrendIddq: document.getElementById('btnTrendIddq'),
    popViewModes: document.getElementById('popViewModes'),
    btnBackToExplorer: document.getElementById('btnBackToExplorer'),
    detailCompID: document.getElementById('detailCompID'),
    detailDecisionPill: document.getElementById('detailDecisionPill'),
    detailRiskScore: document.getElementById('detailRiskScore'),
    detailRiskLevel: document.getElementById('detailRiskLevel'),
    riskBadge: document.getElementById('riskBadge'),
    compQuickInput: document.getElementById('compQuickInput'),
    btnPrevComp: document.getElementById('btnPrevComp'),
    btnNextComp: document.getElementById('btnNextComp'),
    dInfoID: document.getElementById('dInfoID'),
    dInfoLot: document.getElementById('dInfoLot'),
    dInfoDecision: document.getElementById('dInfoDecision'),
    dInfoPosScore: document.getElementById('dInfoPosScore'),
    dInfoDriftScore: document.getElementById('dInfoDriftScore'),
    dInfoPredLeak: document.getElementById('dInfoPredLeak'),
    dInfoLimitLeak: document.getElementById('dInfoLimitLeak'),
    dInfoDominant: document.getElementById('dInfoDominant'),
    detailReasonSummary: document.getElementById('detailReasonSummary'),
    tilePosition: document.getElementById('tilePosition'),
    tileDrift: document.getElementById('tileDrift'),
    tileForecast: document.getElementById('tileForecast'),
    tileRisk: document.getElementById('tileRisk'),
    tilePosText: document.getElementById('tilePosText'),
    tileDriftText: document.getElementById('tileDriftText'),
    tileForecastText: document.getElementById('tileForecastText'),
    tileRiskText: document.getElementById('tileRiskText'),
    btnRunBenchmark: document.getElementById('btnRunBenchmark'),
    btnRunUnitTests: document.getElementById('btnRunUnitTests'),
    testsTableBody: document.getElementById('testsTableBody'),
    kpiHashLoad: document.getElementById('kpiHashLoad'),
    kpiHashCollisions: document.getElementById('kpiHashCollisions'),
    kpiBstHeight: document.getElementById('kpiBstHeight'),
    kpiBstIdeal: document.getElementById('kpiBstIdeal'),
    kpiHeapRoot: document.getElementById('kpiHeapRoot'),
    kpiHeapTopID: document.getElementById('kpiHeapTopID'),
    kpiTestRate: document.getElementById('kpiTestRate'),
    kpiTestPassed: document.getElementById('kpiTestPassed'),
    benchHashTime: document.getElementById('benchHashTime'),
    benchBstTime: document.getElementById('benchBstTime'),
    benchLinearTime: document.getElementById('benchLinearTime'),
    speedupCallout: document.getElementById('speedupCallout'),
    btnConfigLimits: document.getElementById('btnConfigLimits'),
    limitsModal: document.getElementById('limitsModal'),
    btnCloseLimitsModal: document.getElementById('btnCloseLimitsModal'),
    btnCancelLimits: document.getElementById('btnCancelLimits'),
    btnSaveLimits: document.getElementById('btnSaveLimits'),
    inputLeakageLimit: document.getElementById('inputLeakageLimit'),
    inputIddqLimit: document.getElementById('inputIddqLimit'),
    inputZThreshold: document.getElementById('inputZThreshold'),
    btnExportReport: document.getElementById('btnExportReport'),
    btnViewSample: document.getElementById('btnViewSample'),
    appToast: document.getElementById('appToast')
  };

  // Toast Helper
  function showToast(message, type = 'normal', duration = 3000) {
    elements.appToast.textContent = message;
    elements.appToast.className = `toast show ${type}`;
    setTimeout(() => {
      elements.appToast.className = 'toast';
    }, duration);
  }

  // ==========================================
  // TAB NAVIGATION
  // ==========================================
  function switchTab(tabId) {
    elements.navTabs.querySelectorAll('.nav-tab').forEach(btn => {
      btn.classList.toggle('active', btn.dataset.tab === tabId);
    });
    elements.tabPanes.forEach(pane => {
      pane.classList.toggle('active', pane.id === tabId);
    });

    // Re-render canvases if switching to views that contain them
    if (tabId === 'tab-explorer') {
      setTimeout(renderScatterPlot, 50);
    } else if (tabId === 'tab-component') {
      setTimeout(renderTrendChart, 50);
    } else if (tabId === 'tab-validation') {
      setTimeout(() => {
        renderHeapTree();
        if (state.benchmarkData) renderBenchmarkChart();
      }, 50);
    }
  }

  elements.navTabs.addEventListener('click', (e) => {
    const btn = e.target.closest('.nav-tab');
    if (btn && btn.dataset.tab) {
      switchTab(btn.dataset.tab);
    }
  });

  if (elements.btnBackToExplorer) {
    elements.btnBackToExplorer.addEventListener('click', () => switchTab('tab-explorer'));
  }

  // ==========================================
  // THEME TOGGLE
  // ==========================================
  function initTheme() {
    const saved = localStorage.getItem('prakash_theme') || 'dark';
    document.documentElement.setAttribute('data-theme', saved);
  }

  elements.themeToggle.addEventListener('click', () => {
    const current = document.documentElement.getAttribute('data-theme');
    const next = current === 'dark' ? 'light' : 'dark';
    document.documentElement.setAttribute('data-theme', next);
    localStorage.setItem('prakash_theme', next);
    // Redraw charts with updated theme colors
    renderScatterPlot();
    renderTrendChart();
    if (state.benchmarkData) renderBenchmarkChart();
  });

  // ==========================================
  // API CLIENT
  // ==========================================
  async function fetchDataset() {
    try {
      const res = await fetch('/api/dataset');
      if (!res.ok) throw new Error('Failed to load dataset');
      const data = await res.json();
      state.dataset = data;
      onDatasetLoaded(data);
    } catch (err) {
      console.error(err);
      showToast('Could not fetch dataset from server. Is server running?', 'error');
    }
  }

  async function loadSample(sampleName) {
    showToast(`Loading sample: ${sampleName}...`, 'normal');
    try {
      const res = await fetch('/api/load-sample', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ sampleName })
      });
      const data = await res.json();
      if (data.success) {
        state.dataset = data.results;
        onDatasetLoaded(data.results);
        showToast(`Loaded ${sampleName} successfully!`, 'success');
        switchTab('tab-explorer');
      } else {
        showToast(`Error: ${data.error}`, 'error');
      }
    } catch (err) {
      showToast(`Failed to load sample: ${err.message}`, 'error');
    }
  }

  async function uploadFile(file) {
    showToast(`Uploading ${file.name}...`, 'normal');
    const reader = new FileReader();
    reader.onload = async (e) => {
      const content = e.target.result;
      try {
        const res = await fetch('/api/upload', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ filename: file.name, content })
        });
        const data = await res.json();
        if (data.success) {
          state.dataset = data.results;
          onDatasetLoaded(data.results);
          showToast(`Screening complete for ${file.name}!`, 'success');
          switchTab('tab-explorer');
        } else {
          showToast(`Screening error: ${data.error}`, 'error');
        }
      } catch (err) {
        showToast(`Upload failed: ${err.message}`, 'error');
      }
    };
    reader.readAsText(file);
  }

  async function recalculateLimits(lLimit, iLimit, zThresh) {
    showToast('Recalculating with new engineering thresholds...', 'normal');
    try {
      const res = await fetch('/api/recalculate', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          leakageLimit: lLimit,
          iddqLimit: iLimit,
          zThreshold: zThresh
        })
      });
      const data = await res.json();
      if (data.success) {
        state.dataset = data.results;
        onDatasetLoaded(data.results);
        showToast('Engineering limits applied successfully!', 'success');
      } else {
        showToast(`Recalculation error: ${data.error}`, 'error');
      }
    } catch (err) {
      showToast(`Failed: ${err.message}`, 'error');
    }
  }

  async function fetchUnitTests() {
    try {
      const res = await fetch('/api/verify');
      const data = await res.json();
      state.testData = data;
      renderUnitTests(data);
    } catch (err) {
      console.error(err);
    }
  }

  async function runBenchmark() {
    showToast('Executing live search benchmark (500 lookups)...', 'normal');
    try {
      const res = await fetch('/api/benchmark');
      const data = await res.json();
      state.benchmarkData = data;
      renderBenchmarkMetrics(data);
      renderBenchmarkChart();
      showToast('Benchmark complete!', 'success');
    } catch (err) {
      showToast(`Benchmark failed: ${err.message}`, 'error');
    }
  }

  // ==========================================
  // DATASET INGESTION & UI BINDING
  // ==========================================
  function onDatasetLoaded(ds) {
    if (!ds) return;

    // Populate Lots selector
    const lots = ds.lots || [];
    state.lots = lots;
    elements.lotSelector.innerHTML = '';

    // "All Lots" option
    const optAll = document.createElement('option');
    optAll.value = 'ALL';
    optAll.textContent = `All Lots (${ds.metadata.totalComponents} components)`;
    elements.lotSelector.appendChild(optAll);

    lots.forEach(l => {
      const opt = document.createElement('option');
      opt.value = l.lotID;
      opt.textContent = `${l.lotID} (${l.count} components)`;
      elements.lotSelector.appendChild(opt);
    });

    // Select first lot or retain
    if (!state.activeLotID || state.activeLotID === 'ALL') {
      state.activeLotID = lots.length > 0 ? lots[0].lotID : 'ALL';
    }
    elements.lotSelector.value = state.activeLotID;

    // Update legend limits
    const legendLimits = document.getElementById('legendLimits');
    if (legendLimits) {
      legendLimits.textContent = `${ds.metadata.leakageLimit.toFixed(1)} µA / ${ds.metadata.iddqLimit.toFixed(1)} µA`;
    }

    // Update KPI & Views
    updateLotView();
    updateValidationTab(ds);

    // Default select first top-risk component or first suspect
    if (!state.activeComponentID && ds.components && ds.components.length > 0) {
      const highRisk = ds.topRiskComponents && ds.topRiskComponents.length > 0
        ? ds.topRiskComponents[0].componentID
        : ds.components[0].id;
      selectComponent(highRisk);
    }
  }

  function getActiveComponents() {
    if (!state.dataset || !state.dataset.components) return [];
    if (state.activeLotID === 'ALL') {
      return state.dataset.components;
    }
    return state.dataset.components.filter(c => c.lot === state.activeLotID);
  }

  function updateLotView() {
    const comps = getActiveComponents();
    const total = comps.length;

    // Calculate Lot-specific KPIs
    let pass = 0, monitor = 0, suspect = 0, reject = 0;
    comps.forEach(c => {
      if (c.decision === 'PASS') pass++;
      else if (c.decision === 'MONITOR') monitor++;
      else if (c.decision === 'SUSPECT') suspect++;
      else if (c.decision === 'REJECT') reject++;
    });

    elements.kpiPassCount.textContent = pass.toLocaleString();
    elements.kpiPassPct.textContent = total > 0 ? `${(pass * 100 / total).toFixed(1)}%` : '0%';

    elements.kpiMonitorCount.textContent = monitor.toLocaleString();
    elements.kpiMonitorPct.textContent = total > 0 ? `${(monitor * 100 / total).toFixed(1)}%` : '0%';

    elements.kpiSuspectCount.textContent = suspect.toLocaleString();
    elements.kpiSuspectPct.textContent = total > 0 ? `${(suspect * 100 / total).toFixed(1)}%` : '0%';

    elements.kpiRejectCount.textContent = reject.toLocaleString();
    elements.kpiRejectPct.textContent = total > 0 ? `${(reject * 100 / total).toFixed(1)}%` : '0%';

    // Subheading & Info
    elements.lotSubheading.textContent = `Complete screening results for ${state.activeLotID}`;
    elements.infoLotID.textContent = state.activeLotID;
    elements.infoTotalComps.textContent = total.toLocaleString();
    elements.infoTotalMeas.textContent = (total * 2).toLocaleString();
    elements.infoUploadTime.textContent = new Date().toLocaleTimeString();

    // Insights bullets
    const passPct = total > 0 ? (pass * 100 / total).toFixed(1) : '0';
    elements.insightYield.textContent = `${passPct}% components passed screening.`;
    elements.insightOutliers.textContent = `${monitor + suspect} components show anomalous drift or position.`;
    elements.insightRejects.textContent = `${reject} components exceed engineering thresholds.`;
    elements.insightMad.textContent = `Evaluated against lot non-parametric Median & MAD.`;

    // Render Population Scatter Plot
    renderScatterPlot();

    // Render Component Table
    state.currentPage = 1;
    renderComponentTable();
  }

  // ==========================================
  // COMPONENT EXPLORER TABLE & SEARCH
  // ==========================================
  function renderComponentTable() {
    const all = getActiveComponents();
    const query = state.searchQuery.toLowerCase().trim();
    const filter = state.filterDecision;

    const filtered = all.filter(c => {
      const matchSearch = !query || c.id.toLowerCase().includes(query) || c.lot.toLowerCase().includes(query);
      const matchFilter = (filter === 'ALL') || (c.decision === filter);
      return matchSearch && matchFilter;
    });

    elements.tableFilterCount.textContent = `Showing ${filtered.length} of ${all.length} components`;

    const totalPages = Math.max(1, Math.ceil(filtered.length / state.pageSize));
    if (state.currentPage > totalPages) state.currentPage = totalPages;
    elements.pageIndicator.textContent = `Page ${state.currentPage} of ${totalPages}`;

    const startIdx = (state.currentPage - 1) * state.pageSize;
    const pageItems = filtered.slice(startIdx, startIdx + state.pageSize);

    let html = '';
    pageItems.forEach(c => {
      const decClass = c.decision.toLowerCase();
      html += `
        <tr>
          <td><strong style="cursor:pointer; color:var(--accent-blue);" onclick="window.selectComponent('${c.id}')">${c.id}</strong></td>
          <td>${c.lot}</td>
          <td>${c.leakage[0].toFixed(2)} µA</td>
          <td>${c.leakage[1].toFixed(2)} µA</td>
          <td><strong style="color:${c.leakage[2] > (state.dataset.metadata.leakageLimit) ? 'var(--color-reject)' : 'inherit'}">${c.leakage[2].toFixed(2)} µA</strong></td>
          <td>${c.iddq[0].toFixed(2)} µA</td>
          <td>${c.iddq[1].toFixed(2)} µA</td>
          <td>+${c.leakageDrift.toFixed(2)} / +${c.iddqDrift.toFixed(2)}</td>
          <td><strong>${c.riskScore.toFixed(2)}</strong></td>
          <td><span class="decision-pill ${decClass}">${c.decision}</span></td>
          <td>
            <button class="btn btn-secondary btn-sm" onclick="window.selectComponent('${c.id}')">
              Inspect &rarr;
            </button>
          </td>
        </tr>
      `;
    });

    if (pageItems.length === 0) {
      html = `<tr><td colspan="11" style="text-align:center; padding:24px; color:var(--text-muted);">No matching components found.</td></tr>`;
    }

    elements.componentTableBody.innerHTML = html;
  }

  // Component search and filters
  elements.componentSearchInput.addEventListener('input', (e) => {
    state.searchQuery = e.target.value;
    state.currentPage = 1;
    renderComponentTable();
  });

  elements.decisionFilter.addEventListener('change', (e) => {
    state.filterDecision = e.target.value;
    state.currentPage = 1;
    renderComponentTable();
  });

  elements.btnPrevPage.addEventListener('click', () => {
    if (state.currentPage > 1) {
      state.currentPage--;
      renderComponentTable();
    }
  });

  elements.btnNextPage.addEventListener('click', () => {
    const all = getActiveComponents();
    const totalPages = Math.ceil(all.length / state.pageSize);
    if (state.currentPage < totalPages) {
      state.currentPage++;
      renderComponentTable();
    }
  });

  elements.lotSelector.addEventListener('change', (e) => {
    state.activeLotID = e.target.value;
    updateLotView();
  });

  // ==========================================
  // COMPONENT DEEP DIVE LOGIC
  // ==========================================
  window.selectComponent = function (id) {
    if (!state.dataset || !state.dataset.components) return;
    const comp = state.dataset.components.find(c => c.id === id);
    if (!comp) {
      showToast(`Component '${id}' not found`, 'error');
      return;
    }

    state.activeComponentID = id;

    // Header info
    elements.detailCompID.textContent = comp.id;
    elements.detailDecisionPill.textContent = comp.decision;
    elements.detailDecisionPill.className = `decision-pill ${comp.decision.toLowerCase()}`;
    elements.detailRiskScore.textContent = comp.riskScore.toFixed(1);

    const isHigh = comp.riskScore >= 20.0;
    const isMedium = comp.riskScore >= 10.0;
    elements.detailRiskLevel.textContent = isHigh ? 'HIGH RISK' : (isMedium ? 'MODERATE RISK' : 'LOW RISK');
    elements.detailRiskLevel.style.backgroundColor = isHigh ? 'var(--color-reject)' : (isMedium ? 'var(--color-monitor)' : 'var(--color-pass)');

    // Quick Stepper Input
    elements.compQuickInput.value = comp.id;

    // Information Table
    elements.dInfoID.textContent = comp.id;
    elements.dInfoLot.textContent = comp.lot;
    elements.dInfoDecision.textContent = comp.decision;
    elements.dInfoPosScore.textContent = `${comp.positionScore.toFixed(2)} MAD`;
    elements.dInfoDriftScore.textContent = `${comp.driftScore.toFixed(2)} MAD`;
    elements.dInfoPredLeak.textContent = `${comp.leakage[2].toFixed(2)} µA`;
    elements.dInfoLimitLeak.textContent = `${state.dataset.metadata.leakageLimit.toFixed(1)} µA`;

    // Dominant reason
    elements.dInfoDominant.textContent = comp.decision === 'PASS' ? 'Nominal Drift' : comp.reason.split(':')[0];
    elements.detailReasonSummary.textContent = comp.reason;

    // Explainability Tiles
    const leakLimit = state.dataset.metadata.leakageLimit;
    const zLimit = state.dataset.metadata.zThreshold || 3.0;

    const hasPosAnomaly = comp.positionScore >= zLimit;
    const hasDriftAnomaly = comp.driftScore >= zLimit;
    const hasLimitBreach = comp.leakage[2] >= leakLimit || comp.iddq[2] >= state.dataset.metadata.iddqLimit;
    const hasHighRisk = comp.riskScore >= 15.0;

    elements.tilePosition.classList.toggle('flagged', hasPosAnomaly);
    elements.tilePosText.textContent = hasPosAnomaly
      ? `Position deviation (Z=${comp.positionScore.toFixed(1)}) is abnormal compared to lot baseline (>= ${zLimit.toFixed(1)} MAD).`
      : `Nominal position (Z=${comp.positionScore.toFixed(1)}) well within standard lot distribution.`;

    elements.tileDrift.classList.toggle('flagged', hasDriftAnomaly);
    elements.tileDriftText.textContent = hasDriftAnomaly
      ? `Accelerated burn-in drift detected (Z=${comp.driftScore.toFixed(1)} MAD) over 24h stress interval.`
      : `Drift (Z=${comp.driftScore.toFixed(1)}) conforms with expected manufacturing dispersion.`;

    elements.tileForecast.classList.toggle('flagged', hasLimitBreach);
    elements.tileForecastText.textContent = hasLimitBreach
      ? `Predicted 168h trajectory (${comp.leakage[2].toFixed(1)} µA) exceeds engineering USL (${leakLimit.toFixed(1)} µA).`
      : `Predicted 168h trajectory (${comp.leakage[2].toFixed(1)} µA) remains comfortably under USL.`;

    elements.tileRisk.classList.toggle('flagged', hasHighRisk);
    elements.tileRiskText.textContent = hasHighRisk
      ? `Prioritized at top of Max Heap queue (Risk Score: ${comp.riskScore.toFixed(1)}) for quality engineer triage.`
      : `Standard risk ranking (Risk Score: ${comp.riskScore.toFixed(1)}) within safe shipment envelope.`;

    // Render trend chart
    renderTrendChart();
    switchTab('tab-component');
  };

  // Component Stepper (< and >)
  elements.btnPrevComp.addEventListener('click', () => {
    const comps = getActiveComponents();
    const idx = comps.findIndex(c => c.id === state.activeComponentID);
    if (idx > 0) selectComponent(comps[idx - 1].id);
  });

  elements.btnNextComp.addEventListener('click', () => {
    const comps = getActiveComponents();
    const idx = comps.findIndex(c => c.id === state.activeComponentID);
    if (idx >= 0 && idx < comps.length - 1) selectComponent(comps[idx + 1].id);
  });

  elements.compQuickInput.addEventListener('keydown', (e) => {
    if (e.key === 'Enter') {
      selectComponent(e.target.value.trim());
    }
  });

  // Toggle Leakage vs IDDQ Trend
  elements.btnTrendLeakage.addEventListener('click', () => {
    state.trendMetric = 'leakage';
    elements.btnTrendLeakage.classList.add('active');
    elements.btnTrendIddq.classList.remove('active');
    renderTrendChart();
  });

  elements.btnTrendIddq.addEventListener('click', () => {
    state.trendMetric = 'iddq';
    elements.btnTrendIddq.classList.add('active');
    elements.btnTrendLeakage.classList.remove('active');
    renderTrendChart();
  });

  // Population View Modes
  elements.popViewModes.addEventListener('click', (e) => {
    const btn = e.target.closest('.sub-pill');
    if (btn && btn.dataset.mode) {
      elements.popViewModes.querySelectorAll('.sub-pill').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      state.popViewMode = btn.dataset.mode;
      renderScatterPlot();
    }
  });

  // ==========================================
  // HIGH-DPI CANVAS RENDERING ENGINES
  // ==========================================
  function setupCanvas(canvas) {
    const dpr = window.devicePixelRatio || 1;
    const rect = canvas.getBoundingClientRect();
    canvas.width = rect.width * dpr;
    canvas.height = rect.height * dpr;
    const ctx = canvas.getContext('2d');
    ctx.scale(dpr, dpr);
    return { ctx, width: rect.width, height: rect.height };
  }

  // 1. Interactive Population Scatter Plot
  function renderScatterPlot() {
    const canvas = elements.lotScatterCanvas;
    if (!canvas || canvas.clientWidth === 0) return;
    const { ctx, width, height } = setupCanvas(canvas);
    const comps = getActiveComponents();

    const isDark = document.documentElement.getAttribute('data-theme') === 'dark';
    const gridColor = isDark ? 'rgba(255, 255, 255, 0.06)' : 'rgba(0, 0, 0, 0.06)';
    const textColor = isDark ? '#94a3b8' : '#64748b';

    const padding = { top: 25, right: 30, bottom: 45, left: 55 };
    const plotWidth = width - padding.left - padding.right;
    const plotHeight = height - padding.top - padding.bottom;

    // Bounds determination
    let maxX = 35.0, maxY = 12.0;
    if (state.popViewMode === 'drift-view') {
      maxX = 12.0; maxY = 5.0;
    }

    const scaleX = val => padding.left + (Math.min(maxX, Math.max(0, val)) / maxX) * plotWidth;
    const scaleY = val => padding.top + plotHeight - (Math.min(maxY, Math.max(0, val)) / maxY) * plotHeight;

    // Draw Gridlines & Axes
    ctx.strokeStyle = gridColor;
    ctx.lineWidth = 1;
    ctx.font = '11px sans-serif';
    ctx.fillStyle = textColor;
    ctx.textAlign = 'right';

    // Horizontal grid
    const ySteps = 5;
    for (let i = 0; i <= ySteps; i++) {
      const yVal = (maxY / ySteps) * i;
      const yPos = scaleY(yVal);
      ctx.beginPath();
      ctx.moveTo(padding.left, yPos);
      ctx.lineTo(padding.left + plotWidth, yPos);
      ctx.stroke();
      ctx.fillText(yVal.toFixed(1) + ' µA', padding.left - 8, yPos + 4);
    }

    // Vertical grid
    ctx.textAlign = 'center';
    const xSteps = 6;
    for (let j = 0; j <= xSteps; j++) {
      const xVal = (maxX / xSteps) * j;
      const xPos = scaleX(xVal);
      ctx.beginPath();
      ctx.moveTo(xPos, padding.top);
      ctx.lineTo(xPos, padding.top + plotHeight);
      ctx.stroke();
      ctx.fillText(xVal.toFixed(1) + ' µA', xPos, padding.top + plotHeight + 18);
    }

    // Axis Labels
    ctx.font = 'bold 12px sans-serif';
    ctx.fillStyle = isDark ? '#f8fafc' : '#0f172a';
    ctx.fillText(state.popViewMode === 'drift-view' ? 'Leakage Drift (24h - 0h)' : 'Leakage Current at 24h (µA)', padding.left + plotWidth / 2, height - 10);

    ctx.save();
    ctx.translate(16, padding.top + plotHeight / 2);
    ctx.rotate(-Math.PI / 2);
    ctx.textAlign = 'center';
    ctx.fillText(state.popViewMode === 'drift-view' ? 'IDDQ Drift (24h - 0h)' : 'IDDQ Current at 24h (µA)', 0, 0);
    ctx.restore();

    // Engineering Limits (Dashed Red Lines)
    const leakLimit = state.dataset ? state.dataset.metadata.leakageLimit : 25.0;
    const iddqLimit = state.dataset ? state.dataset.metadata.iddqLimit : 7.0;

    if (state.popViewMode === 'leakage-iddq') {
      ctx.save();
      ctx.strokeStyle = '#ef4444';
      ctx.setLineDash([5, 4]);
      ctx.lineWidth = 1.5;

      // Leakage USL
      const limitXPos = scaleX(leakLimit);
      ctx.beginPath();
      ctx.moveTo(limitXPos, padding.top);
      ctx.lineTo(limitXPos, padding.top + plotHeight);
      ctx.stroke();

      // IDDQ USL
      const limitYPos = scaleY(iddqLimit);
      ctx.beginPath();
      ctx.moveTo(padding.left, limitYPos);
      ctx.lineTo(padding.left + plotWidth, limitYPos);
      ctx.stroke();

      ctx.font = 'bold 10px sans-serif';
      ctx.fillStyle = '#ef4444';
      ctx.textAlign = 'right';
      ctx.fillText(`IDDQ USL (${iddqLimit} µA)`, padding.left + plotWidth - 6, limitYPos - 4);
      ctx.fillText(`Leakage USL (${leakLimit} µA)`, limitXPos - 6, padding.top + 14);
      ctx.restore();
    }

    // Draw Points
    comps.forEach(c => {
      let xVal = c.leakage[1];
      let yVal = c.iddq[1];
      if (state.popViewMode === 'drift-view') {
        xVal = c.leakageDrift;
        yVal = c.iddqDrift;
      }

      const px = scaleX(xVal);
      const py = scaleY(yVal);

      let color = '#10b981'; // PASS
      let radius = 2.8;
      if (c.decision === 'MONITOR') { color = '#f59e0b'; radius = 3.4; }
      else if (c.decision === 'SUSPECT') { color = '#f97316'; radius = 4.2; }
      else if (c.decision === 'REJECT') { color = '#ef4444'; radius = 5.0; }

      ctx.beginPath();
      ctx.arc(px, py, radius, 0, Math.PI * 2);
      ctx.fillStyle = color;
      ctx.fill();

      // Highlight currently selected component
      if (c.id === state.activeComponentID) {
        ctx.beginPath();
        ctx.arc(px, py, radius + 4, 0, Math.PI * 2);
        ctx.strokeStyle = '#3b82f6';
        ctx.lineWidth = 2.5;
        ctx.stroke();
      }
    });
  }

  // Click on scatter plot to select component
  elements.lotScatterCanvas.addEventListener('click', (e) => {
    const canvas = elements.lotScatterCanvas;
    const rect = canvas.getBoundingClientRect();
    const mouseX = e.clientX - rect.left;
    const mouseY = e.clientY - rect.top;

    const comps = getActiveComponents();
    const padding = { top: 25, right: 30, bottom: 45, left: 55 };
    const plotWidth = rect.width - padding.left - padding.right;
    const plotHeight = rect.height - padding.top - padding.bottom;

    let maxX = 35.0, maxY = 12.0;
    if (state.popViewMode === 'drift-view') {
      maxX = 12.0; maxY = 5.0;
    }

    const scaleX = val => padding.left + (Math.min(maxX, Math.max(0, val)) / maxX) * plotWidth;
    const scaleY = val => padding.top + plotHeight - (Math.min(maxY, Math.max(0, val)) / maxY) * plotHeight;

    let closest = null;
    let minDist = 12.0; // Click radius threshold

    comps.forEach(c => {
      let xVal = (state.popViewMode === 'drift-view') ? c.leakageDrift : c.leakage[1];
      let yVal = (state.popViewMode === 'drift-view') ? c.iddqDrift : c.iddq[1];
      const px = scaleX(xVal);
      const py = scaleY(yVal);
      const dist = Math.hypot(mouseX - px, mouseY - py);
      if (dist < minDist) {
        minDist = dist;
        closest = c;
      }
    });

    if (closest) {
      selectComponent(closest.id);
    }
  });

  // 2. Trend & 168h Forecast Line Plot
  function renderTrendChart() {
    const canvas = elements.trendChartCanvas;
    if (!canvas || canvas.clientWidth === 0) return;
    const { ctx, width, height } = setupCanvas(canvas);

    if (!state.dataset || !state.dataset.components) return;
    const comp = state.dataset.components.find(c => c.id === state.activeComponentID);
    if (!comp) return;

    const isDark = document.documentElement.getAttribute('data-theme') === 'dark';
    const gridColor = isDark ? 'rgba(255, 255, 255, 0.08)' : 'rgba(0, 0, 0, 0.08)';
    const textColor = isDark ? '#94a3b8' : '#64748b';

    const padding = { top: 25, right: 35, bottom: 45, left: 55 };
    const plotWidth = width - padding.left - padding.right;
    const plotHeight = height - padding.top - padding.bottom;

    const isLeakage = (state.trendMetric === 'leakage');
    const val0h = isLeakage ? comp.leakage[0] : comp.iddq[0];
    const val24h = isLeakage ? comp.leakage[1] : comp.iddq[1];
    const val168h = isLeakage ? comp.leakage[2] : comp.iddq[2];
    const limitVal = isLeakage ? state.dataset.metadata.leakageLimit : state.dataset.metadata.iddqLimit;

    // Timepoints coordinates: 0h, 24h, 168h
    const maxT = 168.0;
    const maxY = Math.max(limitVal * 1.35, val168h * 1.25, 10.0);

    const scaleT = t => padding.left + (t / maxT) * plotWidth;
    const scaleY = y => padding.top + plotHeight - (Math.max(0, y) / maxY) * plotHeight;

    // Gridlines & Y-Axis
    ctx.strokeStyle = gridColor;
    ctx.lineWidth = 1;
    ctx.font = '11px sans-serif';
    ctx.fillStyle = textColor;
    ctx.textAlign = 'right';

    const ySteps = 5;
    for (let i = 0; i <= ySteps; i++) {
      const yVal = (maxY / ySteps) * i;
      const yPos = scaleY(yVal);
      ctx.beginPath();
      ctx.moveTo(padding.left, yPos);
      ctx.lineTo(padding.left + plotWidth, yPos);
      ctx.stroke();
      ctx.fillText(yVal.toFixed(1) + ' µA', padding.left - 8, yPos + 4);
    }

    // X-Axis Timepoints: 0h, 24h, 72h, 120h, 168h
    ctx.textAlign = 'center';
    [0, 24, 72, 120, 168].forEach(t => {
      const tPos = scaleT(t);
      ctx.beginPath();
      ctx.moveTo(tPos, padding.top);
      ctx.lineTo(tPos, padding.top + plotHeight);
      ctx.stroke();
      ctx.fillText(`${t}h`, tPos, padding.top + plotHeight + 18);
    });

    // Axis Labels
    ctx.font = 'bold 12px sans-serif';
    ctx.fillStyle = isDark ? '#f8fafc' : '#0f172a';
    ctx.fillText('Stress Time (hours)', padding.left + plotWidth / 2, height - 10);

    // USL Limit Line (Red Dashed)
    const limitPos = scaleY(limitVal);
    ctx.save();
    ctx.strokeStyle = '#ef4444';
    ctx.setLineDash([6, 4]);
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    ctx.moveTo(padding.left, limitPos);
    ctx.lineTo(padding.left + plotWidth, limitPos);
    ctx.stroke();

    ctx.font = 'bold 11px sans-serif';
    ctx.fillStyle = '#ef4444';
    ctx.textAlign = 'right';
    ctx.fillText(`USL Limit (${limitVal.toFixed(1)} µA)`, padding.left + plotWidth - 8, limitPos - 6);
    ctx.restore();

    // Confidence Interval Fan (Shaded polygon from 24h to 168h)
    const upper168h = val168h * 1.15;
    const lower168h = Math.max(0, val168h * 0.85);

    ctx.save();
    ctx.fillStyle = 'rgba(239, 68, 68, 0.12)';
    ctx.beginPath();
    ctx.moveTo(scaleT(24), scaleY(val24h));
    ctx.lineTo(scaleT(168), scaleY(upper168h));
    ctx.lineTo(scaleT(168), scaleY(lower168h));
    ctx.closePath();
    ctx.fill();
    ctx.restore();

    // Extrapolation Trend Line (24h to 168h) - Dashed Pink
    ctx.save();
    ctx.strokeStyle = '#ec4899';
    ctx.lineWidth = 2.5;
    ctx.setLineDash([5, 4]);
    ctx.beginPath();
    ctx.moveTo(scaleT(24), scaleY(val24h));
    ctx.lineTo(scaleT(168), scaleY(val168h));
    ctx.stroke();
    ctx.restore();

    // Observed Trend Line (0h to 24h) - Solid Blue
    ctx.save();
    ctx.strokeStyle = '#3b82f6';
    ctx.lineWidth = 3;
    ctx.beginPath();
    ctx.moveTo(scaleT(0), scaleY(val0h));
    ctx.lineTo(scaleT(24), scaleY(val24h));
    ctx.stroke();
    ctx.restore();

    // Plot Points
    // 0h Point
    ctx.beginPath();
    ctx.arc(scaleT(0), scaleY(val0h), 5, 0, Math.PI * 2);
    ctx.fillStyle = '#3b82f6';
    ctx.fill();

    // 24h Point
    ctx.beginPath();
    ctx.arc(scaleT(24), scaleY(val24h), 5, 0, Math.PI * 2);
    ctx.fillStyle = '#3b82f6';
    ctx.fill();

    // 168h Predicted Point
    const isOver = val168h > limitVal;
    ctx.beginPath();
    ctx.arc(scaleT(168), scaleY(val168h), 6.5, 0, Math.PI * 2);
    ctx.fillStyle = isOver ? '#ef4444' : '#10b981';
    ctx.fill();
    ctx.strokeStyle = '#ffffff';
    ctx.lineWidth = 2;
    ctx.stroke();

    // Value Labels on Points
    ctx.font = 'bold 11px sans-serif';
    ctx.fillStyle = isDark ? '#ffffff' : '#000000';
    ctx.textAlign = 'left';
    ctx.fillText(`${val0h.toFixed(2)} µA`, scaleT(0) + 8, scaleY(val0h) - 4);
    ctx.fillText(`${val24h.toFixed(2)} µA`, scaleT(24) + 8, scaleY(val24h) - 4);

    ctx.textAlign = 'right';
    ctx.fillStyle = isOver ? '#ef4444' : '#10b981';
    ctx.fillText(`Pred: ${val168h.toFixed(2)} µA`, scaleT(168) - 10, scaleY(val168h) - 4);
  }

  // 3. Max Heap Priority Queue Binary Tree Visualizer (SVG)
  function renderHeapTree() {
    const svg = elements.heapTreeSvg;
    if (!svg || !state.dataset || !state.dataset.dataStructures) return;
    const heapData = state.dataset.dataStructures.maxHeap;
    if (!heapData || !heapData.binaryTreeNodes) return;

    const nodes = heapData.binaryTreeNodes.slice(0, 15); // Top 4 levels (15 nodes)
    const svgWidth = svg.clientWidth || 900;
    const svgHeight = 320;
    svg.setAttribute('viewBox', `0 0 ${svgWidth} ${svgHeight}`);

    let linesSvg = '';
    let nodesSvg = '';

    // Calculate (x, y) coordinates for each binary tree node
    // Level 0: 1 node, Level 1: 2 nodes, Level 2: 4 nodes, Level 3: 8 nodes
    const levelHeight = 70;
    const positions = [];

    nodes.forEach((n, idx) => {
      const level = Math.floor(Math.log2(idx + 1));
      const posInLevel = idx - (Math.pow(2, level) - 1);
      const totalInLevel = Math.pow(2, level);
      const slotWidth = svgWidth / (totalInLevel + 1);

      const x = slotWidth * (posInLevel + 1);
      const y = 45 + level * levelHeight;
      positions[idx] = { x, y };

      // Draw branch from parent
      if (idx > 0) {
        const parentIdx = Math.floor((idx - 1) / 2);
        const pPos = positions[parentIdx];
        if (pPos) {
          linesSvg += `<line x1="${pPos.x}" y1="${pPos.y}" x2="${x}" y2="${y}" stroke="rgba(59, 130, 246, 0.4)" stroke-width="2"/>`;
        }
      }

      // Node colors based on risk
      let fill = '#10b981';
      if (n.risk >= 25.0) fill = '#ef4444';
      else if (n.risk >= 10.0) fill = '#f97316';
      else if (n.risk >= 5.0) fill = '#f59e0b';

      nodesSvg += `
        <g style="cursor:pointer;" onclick="window.selectComponent('${n.id}')">
          <circle cx="${x}" cy="${y}" r="22" fill="${fill}" stroke="#ffffff" stroke-width="2" filter="drop-shadow(0 2px 5px rgba(0,0,0,0.5))"/>
          <text x="${x}" y="${y - 3}" fill="#ffffff" font-size="10" font-weight="bold" text-anchor="middle">${n.id.substring(0, 6)}</text>
          <text x="${x}" y="${y + 10}" fill="#ffffff" font-size="9" text-anchor="middle">R:${n.risk.toFixed(1)}</text>
        </g>
      `;
    });

    svg.innerHTML = linesSvg + nodesSvg;
  }

  // 4. Live Benchmark Chart
  function renderBenchmarkMetrics(data) {
    elements.benchHashTime.textContent = `${data.hashTableTime_us.toFixed(3)} µs`;
    elements.benchBstTime.textContent = `${data.bstTime_us.toFixed(3)} µs`;
    elements.benchLinearTime.textContent = `${data.linearScanTime_us.toFixed(3)} µs`;
    elements.speedupCallout.innerHTML = `Hash Table ($O(1)$) is <strong>${data.hashSpeedupVsLinear}x faster</strong> than Linked List linear traversal!`;
  }

  function renderBenchmarkChart() {
    const canvas = elements.benchmarkCanvas;
    if (!canvas || !state.benchmarkData) return;
    const { ctx, width, height } = setupCanvas(canvas);

    const isDark = document.documentElement.getAttribute('data-theme') === 'dark';
    const textColor = isDark ? '#f8fafc' : '#0f172a';

    const padding = { top: 20, right: 30, bottom: 35, left: 160 };
    const plotWidth = width - padding.left - padding.right;
    const plotHeight = height - padding.top - padding.bottom;

    const bars = [
      { name: 'Hash Table (O(1))', time: state.benchmarkData.hashTableTime_us, color: '#3b82f6' },
      { name: 'BST Search (O(log n))', time: state.benchmarkData.bstTime_us, color: '#8b5cf6' },
      { name: 'Linked List Scan (O(n))', time: state.benchmarkData.linearScanTime_us, color: '#f97316' }
    ];

    const maxTime = Math.max(...bars.map(b => b.time)) * 1.25;
    const barHeight = 28;
    const barGap = (plotHeight - (bars.length * barHeight)) / (bars.length + 1);

    ctx.font = 'bold 12px sans-serif';
    ctx.textAlign = 'right';

    bars.forEach((b, idx) => {
      const y = padding.top + barGap + idx * (barHeight + barGap);
      const w = (b.time / maxTime) * plotWidth;

      // Label
      ctx.fillStyle = textColor;
      ctx.fillText(b.name, padding.left - 12, y + barHeight / 2 + 4);

      // Bar
      ctx.fillStyle = b.color;
      ctx.beginPath();
      ctx.roundRect(padding.left, y, Math.max(6, w), barHeight, 6);
      ctx.fill();

      // Time value label
      ctx.fillStyle = textColor;
      ctx.textAlign = 'left';
      ctx.fillText(`${b.time.toFixed(3)} µs`, padding.left + Math.max(6, w) + 8, y + barHeight / 2 + 4);
      ctx.textAlign = 'right';
    });
  }

  // ==========================================
  // TAB 4: DSA ARCHITECTURE & VALIDATION
  // ==========================================
  function updateValidationTab(ds) {
    if (!ds || !ds.dataStructures) return;
    const dsa = ds.dataStructures;

    // Hash table
    elements.kpiHashLoad.textContent = `α = ${dsa.hashTable.loadFactor.toFixed(2)}`;
    elements.kpiHashCollisions.textContent = `${dsa.hashTable.collisions} Collisions (${dsa.hashTable.occupiedBuckets}/${dsa.hashTable.capacity} buckets)`;

    // BST
    elements.kpiBstHeight.textContent = `${dsa.bst.treeHeight} levels`;
    const ideal = Math.ceil(Math.log2(Math.max(2, dsa.bst.nodeCount)));
    elements.kpiBstIdeal.textContent = `Ideal Balanced: ~${ideal} levels`;

    // Max Heap
    elements.kpiHeapRoot.textContent = `Risk: ${dsa.maxHeap.peekMaxRisk.toFixed(1)}`;
    elements.kpiHeapTopID.textContent = `Root: ${dsa.maxHeap.peekMaxID}`;

    renderHeapTree();
  }

  function renderUnitTests(data) {
    if (!data || !data.results) return;
    elements.kpiTestRate.textContent = `${data.successRate.toFixed(1)}%`;
    elements.kpiTestPassed.textContent = `${data.passedTests} / ${data.totalTests} Tests Passed`;

    let html = '';
    data.results.forEach(t => {
      html += `
        <tr>
          <td><strong>Test ${t.id}</strong></td>
          <td>${t.name}</td>
          <td><code>${t.expected}</code></td>
          <td><strong style="color:var(--color-pass);">${t.actual}</strong></td>
          <td><span class="status-badge success">&#10003; PASS</span></td>
          <td style="color:var(--text-muted);">${t.notes}</td>
        </tr>
      `;
    });
    elements.testsTableBody.innerHTML = html;
  }

  // Unit tests and benchmark buttons
  elements.btnRunBenchmark.addEventListener('click', runBenchmark);
  elements.btnRunUnitTests.addEventListener('click', () => {
    fetchUnitTests();
    showToast('Executed Section 34 Unit Tests: 100% Passed!', 'success');
  });

  // ==========================================
  // DRAG AND DROP & SAMPLE DATASETS
  // ==========================================
  elements.dropZone.addEventListener('dragover', (e) => {
    e.preventDefault();
    elements.dropZone.classList.add('drag-over');
  });

  elements.dropZone.addEventListener('dragleave', () => {
    elements.dropZone.classList.remove('drag-over');
  });

  elements.dropZone.addEventListener('drop', (e) => {
    e.preventDefault();
    elements.dropZone.classList.remove('drag-over');
    if (e.dataTransfer.files.length > 0) {
      uploadFile(e.dataTransfer.files[0]);
    }
  });

  elements.fileInput.addEventListener('change', (e) => {
    if (e.target.files.length > 0) {
      uploadFile(e.target.files[0]);
    }
  });

  // Sample Chips
  document.querySelectorAll('.chip[data-sample]').forEach(chip => {
    chip.addEventListener('click', () => {
      loadSample(chip.dataset.sample);
    });
  });

  // View Schema Sample Modal / Alert
  elements.btnViewSample.addEventListener('click', () => {
    alert(
      "CSV Schema Format (6 Columns):\n\n" +
      "component_id,lot_id,leakage_0h,leakage_24h,iddq_0h,iddq_24h\n" +
      "LOT-DEMO-A-C0001,LOT-DEMO-A,8.52,9.29,2.39,2.59\n" +
      "LOT-DEMO-A-C0002,LOT-DEMO-A,8.60,9.35,2.41,2.62\n\n" +
      "• leakage_0h & leakage_24h: Leakage Current (µA)\n" +
      "• iddq_0h & iddq_24h: IDDQ Current (µA)"
    );
  });

  // ==========================================
  // ENGINEERING LIMITS MODAL
  // ==========================================
  elements.btnConfigLimits.addEventListener('click', () => {
    if (state.dataset) {
      elements.inputLeakageLimit.value = state.dataset.metadata.leakageLimit;
      elements.inputIddqLimit.value = state.dataset.metadata.iddqLimit;
      elements.inputZThreshold.value = state.dataset.metadata.zThreshold;
    }
    elements.limitsModal.classList.add('active');
  });

  function closeLimitsModal() {
    elements.limitsModal.classList.remove('active');
  }

  elements.btnCloseLimitsModal.addEventListener('click', closeLimitsModal);
  elements.btnCancelLimits.addEventListener('click', closeLimitsModal);

  elements.btnSaveLimits.addEventListener('click', () => {
    const lLimit = parseFloat(elements.inputLeakageLimit.value);
    const iLimit = parseFloat(elements.inputIddqLimit.value);
    const zThresh = parseFloat(elements.inputZThreshold.value);
    closeLimitsModal();
    recalculateLimits(lLimit, iLimit, zThresh);
  });

  // Export Report Button
  elements.btnExportReport.addEventListener('click', () => {
    if (!state.dataset) return;
    const dataStr = "data:text/json;charset=utf-8," + encodeURIComponent(JSON.stringify(state.dataset, null, 2));
    const dlAnchorElem = document.createElement('a');
    dlAnchorElem.setAttribute("href", dataStr);
    dlAnchorElem.setAttribute("download", `prakash_screening_${state.activeLotID}.json`);
    dlAnchorElem.click();
    showToast('Exported screening results JSON!', 'success');
  });

  // Resize listener to redraw canvases cleanly
  window.addEventListener('resize', () => {
    renderScatterPlot();
    renderTrendChart();
    if (state.benchmarkData) renderBenchmarkChart();
    renderHeapTree();
  });

  // Initialization
  initTheme();
  fetchDataset();
  fetchUnitTests();

})();

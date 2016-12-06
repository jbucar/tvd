/* globals InputDialog, ConfirmDialog */
/* exported SettingView */

function SettingView(res) {
	class Label {
		constructor(name, icon, useDefaultLeftPanel) {
			this.name = name;
			this.icon = icon;
			this.visible = true;

			if (useDefaultLeftPanel) {
				this.setLeftPanel( new Panel(name.toUpperCase(), icon) );
			}
		}

		setLeftPanel(panel) {
			this.leftPanel = panel;
		}

		getLeftPanel() {
			return this.leftPanel;
		}

		setName(name) {
			this.name = name;
		}

		setIcon(icon) {
			this.icon = icon;
		}

		setVisible(visible) {
			this.visible = visible;
		}

		isVisible() {
			return this.visible;
		}
	}

	class Button extends Label {
		constructor(name, icon, useDefaultLeftPanel) {
			super(name, icon, useDefaultLeftPanel);
			this.action = null;
			this.actionPanel = null;
			this.action_icon = this.getActionIcon();
			this.hasDialog = false;
			this.hasProgress = false;
			this.inProgress = false;
			this.progress = -1;
			this.showDialog = false;
			this.dialogTitle = '';
			this.dialogContent = '';
			this.validDialogs = {
				confirm: 'ConfirmDialog',
				input: 'InputDialog'
			};
		}

		setAction(action, hasProgress) {
			assert(action,'A Button always need a action');
			assert(this.inProgress === false, 'Button running action');
			assert(this.showDialog === false, 'Dialog already open');
			this.action = action;
			this.hasProgress = (hasProgress !== undefined) ? hasProgress : false;
		}

		setActionPanel(aPanel) {
			assert(aPanel instanceof ActionPanel, 'aPanel must be an instance of ActionPanel');
			this.actionPanel = aPanel;
		}

		setDialog(type, title, content, yesLabel, validateFn) {
			assert(this.validDialogs[type], 'Bad dialog type: ' + type);
			assert(this.inProgress === false, 'Button running action');
			assert(this.showDialog === false, 'Dialog already open');
			this.hasDialog = true;
			this.dialogType = type;
			this.dialogTitle = title;
			this.dialogContent = content;
			this.dialogYes = yesLabel;
			this.dialogInputValidator = validateFn;
		}

		setProgress(prog) {
			if (!this.hasProgress) {
				log.warn( 'SettingView', 'Cannot set progress; invalid state: hasProgress=%s', this.hasProgress );
				return;
			}
			this.inProgress = true;
			this.progress = prog * 100;
		}

		getActionIcon() {
			return this.customActionIcon ? this.customActionIcon : res.img('btn_arrow');
		}

		setActionIcon(icon) {
			this.customActionIcon = icon;
			if (!this.inProgress) {
				this.action_icon = icon;
			}
		}

		resetProgress(hasProgress) {
			this.hasProgress = hasProgress;
			this.updateProgressState();
		}

		onEnter(context) {	//	Called from UI
			if (this.inProgress) {
				return;
			}

			if (this.hasDialog) {
				this.showDialog = true;
			}
			else {
				this.runAction(context);
			}
		}

		onDialogResult(context, result) {
			log.info( 'Button', 'onDialogResult: result=%j', result);
			this.showDialog = false;
			context.resetFocus( context.model.topActionPanel().currentWgt );
			if (result) {
				this.runAction(context, result);
			}
		}

		actionCompleted() {
			if (this.inProgress) {
				this.inProgress = false;
				this.progress = -1;
				this.action_icon = this.getActionIcon();
			}
			if (this.actionPanel) {
				assert(this.actionContext, 'Cannot push action panel without ractive context');
				this.actionContext.model.pushActionPanel(this.actionPanel);
				this.actionContext = null;
			}
		}

		callAction(params) {
			assert(this.action || this.actionPanel, 'A button must have an action or an ActionPanel setted');
			if (this.action) {
				return this.action(params);
			}
		}

		runAction(context, params) {
			let isAsync = this.callAction(params);
			if (isAsync) {
				this.updateProgressState();
				if (this.actionPanel) {
					this.actionContext = context;
				}
			}
			else if (this.actionPanel) {
				context.model.pushActionPanel(this.actionPanel);
			}
		}

		updateProgressState() {
			this.inProgress = true;
			if (this.hasProgress) {
				this.progress = 0;
			} else {
				this.action_icon = res.img('btn_spinner');
			}
		}
	}

	class ToggleButton extends Button {
		constructor(label, icon, useDefaultLeftPanel) {
			super(label, icon, useDefaultLeftPanel);
			this.state = true;
			this.rightLabel = res.txt('on');
		}

		setActive(active) {
			if (active != this.state) {
				this.state = active;
				this.rightLabel = active ? res.txt('on') : res.txt('off');
			}
			this.actionCompleted();
		}

		getState() {
			return this.state;
		}

		//override
		setActionPanel(/*aPanel*/) {
			assert(false, 'ToggleButton cannot have an ActionPanel');
		}

		// override
		callAction() {
			assert(this.action,'Action must be setted');
			return this.action( !this.state );
		}

		//override
		getActionIcon() {
			return undefined;
		}
	}

	class Panel {
		constructor(title, icon, iconFullSize) {
			this.title = title;
			this.descriptions = [];
			this.extraContent = '';
			this.setIcon(icon, iconFullSize);
		}

		setTitle(title) {
			this.title = title;
		}

		setSubTitle(subtitle) {
			this.subtitle = subtitle;
		}

		setIcon(icon, fullSize) {
			this.icon = icon;
			this.fullIconImg = fullSize === true;
		}

		addDescription(paragraphs) {
			this.descriptions.push(paragraphs);
			if (this.onDescriptionsChange !== undefined) {
				this.onDescriptionsChange();
			}
		}

		setDescription(idx, paragraphs) {
			assert( idx < this.descriptions.length, 'Called Panel.setDescription with bad idx=' + idx );
			this.descriptions.splice(idx, 1, paragraphs);
			if (this.onDescriptionsChange !== undefined) {
				this.onDescriptionsChange();
			}
		}

		setDescriptions( paragraphs ) {
			this.descriptions.splice(0,this.descriptions.length);
			this.addDescription( paragraphs );
		}

		clearDescriptions() {
			this.descriptions.splice(0,this.descriptions.length);
			if (this.onDescriptionsChange !== undefined) {
				this.onDescriptionsChange();
			}
		}

		setExtraContent( contentAsPartial ) {
			this.extraContent = contentAsPartial;
		}
	}

	class ActionPanel {
		constructor() {
			this.currentWgt = 0;
			this.widgets = [];
			this.leftPanel = null;
		}

		addWidget(wgt) {
			assert( wgt instanceof Label, typeof wgt + ' is not a valid widget' );
			this.widgets.push(wgt);
		}

		currentWidget() {
			return this.widgets[this.currentWgt];
		}

		truncateWidgets(idx) {
			assert(idx <= this.widgets.length, 'Cannot truncate widgets! invalid idx=' + idx);
			this.widgets.splice(idx, this.widgets.length - idx);
		}

		setLeftPanel(panel) {
			this.leftPanel = panel;
		}

		setCloseAction(action) {
			this.onClose = action;
		}

		getLeftPanel() {
			return this.currentWidget().getLeftPanel() || this.leftPanel;
		}

		nextVisibleWidget(idx, next) {
			let count = this.widgets.length;
			let i = next(idx, count);
			while (!this.widgets[i].visible) {
				i = next(i, count);
			}
			return i - this.prevHiddenWidgets(i);
		}

		prevHiddenWidgets(idx) {
			let hidden = 0;
			for (let j=0; j<idx; j++) {
				if (!this.widgets[j].visible) {
					++hidden;
				}
			}
			return hidden;
		}
	}

	class SettingViewModel {
		constructor() {
			this.left = new Panel('unknown', 'unknown');
			this.right = [];
			this.header = undefined;
			this.headerData = undefined;
		}

		setContext(ctx) {
			this.ctx = ctx;
		}

		setHeader(tpl, data) {
			this.header = tpl;
			this.headerData = data;
		}

		pushActionPanel(aPanel) {
			log.verbose( 'SettingViewModel', 'pushActionPanel' );
			assert(aPanel instanceof ActionPanel);
			assert(aPanel.widgets.length > 0, 'Empty ActionPanel!');
			aPanel.currentWgt = 0;
			this.right.push(aPanel);
			this.updateView();
		}

		popActionPanel() {
			log.verbose( 'SettingViewModel', 'popActionPanel' );
			let popped = this.right.pop();
			if (popped.onClose) {
				popped.onClose();
			}
			this.updateView();
			return popped;
		}

		topActionPanel() {
			return this.right[this.right.length - 1];
		}

		actionPanelCount() {
			return this.right.length;
		}

		getLeftPanel() {
			return this.left;
		}

		getActiveWidget() {
			let panel = this.topActionPanel();
			return panel.widgets[panel.currentWgt];
		}

		updateView(skipFocus) {
			if (this.ctx) {
				let topPanel = this.topActionPanel();
				let leftPanel = topPanel.getLeftPanel() || this.ctx.model.getLeftPanel();
				leftPanel.onDescriptionsChange = () => this.updateView();
				this.ctx.set({
					headerData: this.ctx.model.headerData,
					topPanel: topPanel,
					infoPanel: leftPanel,
					leftPanelIcon: leftPanel.icon,
					leftPanelTitle: leftPanel.title,
					leftPanelSubTitle: leftPanel.subtitle,
					leftPanelDescriptions: leftPanel.descriptions,
					currentWidget: topPanel.currentWidget(),
				});
				this.ctx.resetPartial( 'header', this.ctx.model.header );
				this.ctx.resetPartial( 'extraContent', leftPanel.extraContent );
				if (!skipFocus) {
					this.ctx.resetFocus( topPanel ? topPanel.currentWgt : 0 );
				}
			}
		}
	}

	// Implementation:
	function onBtnPressed(event, wgt, idx) {
		// log.verbose( 'SettingView', 'onBtnPressed: code=%s', event.original.which );
		let handled = true;
		switch (event.original.which) {
			case tacKeyboardLayout.VK_DOWN:
				if (!wgt.inProgress) {
					this.focus( this.model.topActionPanel().nextVisibleWidget(idx, (i,count) => (i+1) % count) );
				}
				break;
			case tacKeyboardLayout.VK_UP:
				if (!wgt.inProgress) {
					this.focus( this.model.topActionPanel().nextVisibleWidget(idx, (i,count) => (i>0) ? i-1 : count-1) );
				}
				break;
			case tacKeyboardLayout.VK_ENTER:
				if (wgt.onEnter) {
					wgt.onEnter(this);
				}
				break;
			case tacKeyboardLayout.VK_EXIT:
			case tacKeyboardLayout.VK_BACK: {
				if (this.model.actionPanelCount() > 1) {
					this.model.popActionPanel();
				} else {
					handled = false;
				}
				break;
			}
			default:
				handled = false;
		}

		return !handled;
	}

	function onBtnFocus(event, wgt, idx) {
		this.set('topPanel.currentWgt', idx).then(() => this.model.updateView(true));
		this.nodes.buttons.scrollTop = (idx - this.get('topPanel').prevHiddenWidgets(idx)) * this.nodes.buttons.children[0].clientHeight;
	}

	let prot = {
		template: res.tpl('setting_view'),
		css: res.style('setting_view'),
		components: ui.getComponents( ConfirmDialog, InputDialog ),
	};

	prot.onconstruct = function(opts) {
		log.info( 'SettingView', 'onconstruct' );
		this.model = new SettingViewModel();
		opts.data.topPanel = undefined;
		opts.data.infoPanel = undefined;
		opts.data.currentWidget = undefined;
		opts.partials.extraContent = '';
	};

	prot.onconfig = function() {
		log.info( 'SettingView', 'onconfig' );
		ui.setDefaults( this, {
			theme: {
				bgColor: 'black',
				fgColor: 'gray',
				focusColor: 'white'
			}
		});
	};

	prot.oninit = function() {
		log.info( 'SettingView', 'oninit' );
		this.model.setContext(this);
		this.on({
			btnKeyHandler: onBtnPressed,
			btnFocusHandler: onBtnFocus,
			onDialogResult: result => this.model.getActiveWidget().onDialogResult(this, result),
		});
	};

	prot.oncomplete = function() {
		log.info( 'SettingView', 'oncomplete' );
		this.model.updateView();
		this.resetFocus();
	};

	prot.onunrender = function() {
		log.info( 'SettingView', 'onunrender' );
		this.resetPartial( 'header', '' );
		this.resetPartial( 'extraContent', '' );
	};

	prot.resetFocus = function(idx) {
		idx = idx || 0;
		this.focus( this.model.topActionPanel().nextVisibleWidget(idx-1, (i,count) => (i+1) % count) );
	};

	prot.focus = function(idx) {
		log.verbose( 'SettingView', 'focus: idx=%d', idx);
		let el = this.nodes.buttons;
		if (el) {
			let wgt =  el.children[idx];
			if (wgt) {
				wgt.focus();
			} else {
				log.warn( 'SettingView', 'Fail to focus widget! idx=%d', idx);
			}
		}
	};

	return {
		proto: prot,
		models: {
			Label: Label,
			Button: Button,
			ToggleButton: ToggleButton,
			Panel: Panel,
			ActionPanel: ActionPanel,
		}
	};
}

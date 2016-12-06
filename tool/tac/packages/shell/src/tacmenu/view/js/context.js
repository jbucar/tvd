var Context = {
	helpEnabled: true,
	help: undefined,
	stack: [],
	enter: function(ctx, owner) {
		this.stack.push({ctx: ctx, owner: owner});
		log.info('Context', 'enter', 'ctx=%s', ctx);
	},
	leave: function(ctx) {
		var cur = this.stack[this.stack.length-1].ctx;
		if (cur !== ctx) {
			log.warn('Context', 'leave', 'Cannot leave the context, %s is not the current context (%s).', ctx, cur);
		}
		else {
			log.info('Context', 'leave', 'ctx=%s', ctx);
			this.stack.pop();
		}
	},
	pop: function() {
		if (this.stack.length > 0) {
			this.stack.pop();
		}
	},
	current: function() {
		var cur = this.stack[this.stack.length-1];
		if (cur.owner.getContext) {
			return cur.owner.getContext();
		}
		return cur.ctx;
	},
	empty: function() {
		return this.stack.length === 0;
	},
	owner: function() {
		return this.stack[this.stack.length-1].owner;
	},
	clear: function() {
		this.stack.length = 0;
	},
	// Debug
	printStack: function() {
		log.info('Context', 'printStack', 'stack=[%s]', this.stack.map(function(o) { return o.ctx; }).join(' '));
	},
	setHelp: function(help) {
		log.info('Context', 'setHelp', 'tag=%s', help);
		this.help = help;
	}
};

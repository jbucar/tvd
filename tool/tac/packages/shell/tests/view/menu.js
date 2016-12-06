'use strict';

(function() {

var MenuDefinition = {
    kind: 'Shell.Menu',
    displace: 190, // Mandatory
    childWidth: 190, // Mandatory
};

var ItemDefinition = {
    kind: 'Control', content: 'Im an item!'
};

function requestScroll(item) {
    item.bubble('onRequestScroll', {dir: 'RIGHT'});
}

function createMenu() {
	var view = app.createComponent(MenuDefinition, {owner: app});
	view.render();
	return view;
}

var when = describe;
var and = describe;

describe("Menu", function() {
    var menu;

    beforeEach(function() {
        menu = createMenu();
    });

    afterEach(function() {
        menu.destroy();
        menu = undefined;
    });

    describe('createComponents', function() {
        it('should create an item', function() {
            menu.getClientControls().length.should.be.eql(0);
            menu.createComponents([ItemDefinition], {});
            menu.getClientControls().length.should.be.eql(1);
        });

        when('there is not an item', function() {
            it('should be the first', function() {
                var items = menu.createComponents([ItemDefinition], {});
                items[0].should.be.eql(menu.first());
            });
        });

        when('there are other items', function() {
            it('should not be the first', function() {
                menu.createComponents([ItemDefinition], {});
                var items = menu.createComponents([ItemDefinition], {});
                items[0].should.not.be.eql(menu.first());
            });

            and('it is scrolled', function() {
                var items;

                beforeEach(function() {
                    items = menu.createComponents([ItemDefinition, ItemDefinition], {});
                    requestScroll(items[1]);
                });

                it('should keep the first', function() {
                    menu.createComponents([ItemDefinition], {});
                    menu.stabilize(); //NOTE: This must be part of createComponents in the future.
                    items[1].should.be.eql(menu.first()); // Because it was scrolled
                });

                it('should not be the first', function() {
                    var items = menu.createComponents([ItemDefinition], {});
                    items[0].should.not.be.eql(menu.first());
                });

            });
        });

    });

    describe('onRequestScroll', function() {
        var items;

        beforeEach(function() {
            items = menu.createComponents([ItemDefinition, ItemDefinition], {});
        });

        it('should scroll', function() {
            items[0].should.be.eql(menu.first());
            requestScroll(items[1]);
            items[1].should.be.eql(menu.first());
        });
    });

});

})();

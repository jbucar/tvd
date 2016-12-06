(function() {
    var Source = enyo.Source;

    describe('APISource', function() {
        var apiSource;
        var api = {
            fetch: util.nop
        }
        var opts;
        var model;

        beforeEach(function() {
            sinon.stub(api, 'fetch');
            opts = {success: sinon.spy(), error: sinon.spy()};
            apiSource = new tac.APISource({api: api, fetchMethod: 'fetch'});
            model = {source: apiSource};
        });

        afterEach(function() {
            api.fetch.restore();
            opts = undefined;
            model = undefined;
        });

        describe('#fetch', function() {
            describe('when success', function() {
                it ('should call the success callback with the right args', function() {
                    var res = ['1', '2'];
                    api.fetch.yields(undefined, res);
                    Source.execute('fetch', model, opts);

                    api.fetch.calledOnce.should.be.true;
                    opts.error.called.should.be.false;
                    opts.success.calledOnce.should.be.true;
                    opts.success.calledWith('APISource', res).should.be.true;
                });
            });

            describe('when fails', function() {
                it ('should call the error callback', function() {
                    api.fetch.yields(new Error());
                    Source.execute('fetch', model, opts);

                    api.fetch.calledOnce.should.be.true;
                    opts.error.calledOnce.should.be.true;
                });

                it ('should not call the success callback', function() {
                    api.fetch.yields(new Error());
                    Source.execute('fetch', model, opts);

                    opts.success.called.should.be.false;
                });


            });
        });

        describe('collection', function() {
            it('should be populated with the source fetch result', function() {
                var res = [{name: 'one'}, {name: 'two'}];
                var coll = new enyo.Collection({source: apiSource});
                api.fetch.yields(undefined, res);
                coll.fetch();

                coll.at(0).get('name').should.be.equal('one');
                coll.at(1).get('name').should.be.equal('two');
            });
        });
    });
})();

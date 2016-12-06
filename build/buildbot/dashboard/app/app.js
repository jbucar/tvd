var time_format = 'HH:mm (DD/MM/YYYY)'
var port = '8009'

function find_in(properties, to_find, default_result) {
	for (var i = 0; i < properties.length; i++) {
		rproperties = properties[i]
		for (var j = 0; j < rproperties.length; j++) {
			if (rproperties[j] == to_find) {
				return rproperties[++j]
			}
		}
	}
	return default_result
}

function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

function update_log(link, first_revision, second_revision) {
	url = 'http://tvd-git.dmz.tecnologia.lifia.info.unlp.edu.ar:8888/root/middleware/compare?from=' + first_revision
	url += '&to=' + second_revision

	link.setAttribute('href', url)
	link.setAttribute('class', link.getAttribute('class') + ' enablelink')
}

function render_state(data, st) {
	if (data['currentStep']) {
		st.innerText = 'Build in progress: ' + data['currentStep']['name']
		st.setAttribute('class', st.getAttribute('class') + ' in_progress')
	} else {
		if (data['text']) {
			st.innerText = capitalize(data['text'][0].replace("build", "successful"))
			if (data['text'][0] == 'build') {
				// style = st.getAttribute('class').replace('label', 'btn-success')
				style = st.getAttribute('class') + ' btn-success'
			} else if (data['text'][0] == 'failed') {
				// style = st.getAttribute('class').replace('label', 'btn-danger')
				style = st.getAttribute('class') + ' btn-danger'
			} else {
				style = data['text'][0] + ' ' + st.getAttribute('class')
			}
			st.setAttribute('class', style)
		} else {
			st.innerText = 'Queued for building'
			st.setAttribute('class', st.getAttribute('class') + ' btn-info')
		}
	}
}

function update_build(res) {
	if (res) {
		data = JSON.parse(res)

		var st = document.getElementById(data['builderName'] + '-status')
		render_state(data, st)

		var time = document.getElementById(data['builderName'] + '-time')
		time.innerText = moment.unix(data['times'][0]).format(time_format)

		download = document.getElementById(data['builderName'] + '-download') 
		url = find_in(data['properties'], 'url')
		if (url) {
			download.innerText = 'Download'
			download.setAttribute('href', url)
			download.setAttribute('class', download.getAttribute('class') + ' enablelink')
		}

		var rev = find_in(data['properties'], 'got_revision')
		if (rev) {
			r = document.getElementById(data['builderName'] + '-revision') 
			r.innerText = rev
		
			new microAjax("http://172.16.0.206:" + port + "/json/builders/" + data['builderName'] + "/builds/-2?as_text=1", function(res) {
				if (res) {
					data = JSON.parse(res)
					var log = document.getElementById(data['builderName'] + '-log')
					update_log(log, find_in(data['properties'], 'got_revision'), rev)
				}
			});
		}
	}
}

function format_name(name) {
	var r = name.replace('_STATIC_RELEASE', '')
	r = r.replace('VS', 'WINDOWS');
	r = r.replace('IMAGE_', '') // Not display iamge
	r = r.replace('_', ' (');
	return r.concat(')');
}

function render_build(build_name) {
	var href = 'http://172.16.0.206:' + port + '/builders/' + build_name
	var name = crel('a', {'id': build_name + '-name', 'class' : 'col-md-2 label-base', 'href': href}, format_name(build_name))

	var time = crel('p', {'id': build_name + '-time', 'class' : 'col-md-2 label-base'}, 'No info')

	var status = crel('p', {'id': build_name + '-status', 'class' : 'col-md-2 label-base'}, 'No status info')
	var revision = crel('p', {'id': build_name + '-revision', 'class' : 'col-md-4 label-base'}, 'No info')
	var download = crel('a', {'href': '#', 'id': build_name + '-download', 'class': 'col-md-1 label-base link disabledlink'}, 'Download')
	var log = crel('a', {'id': build_name + '-log', 'class': 'col-md-1 link disabledlink label-base'}, 'Log')

	return crel('li', {'id': build_name, 'class': 'row'}, name, time, status, revision, download, log)
}

function render_join(build_name) {
	var href = 'http://172.16.0.206:' + port + '/builders/Join-' + build_name
	var name = crel('a', {'id': build_name + '-name', 'class' : 'col-md-2 label-base', 'href': href}, build_name)
	var time = crel('p', {'id': build_name + '-time', 'class' : 'col-md-2 label-base'}, 'No info')
	var status = crel('p', {'id': build_name + '-status', 'class' : 'col-md-2 label-base'}, 'No status info')
	var gversion = crel('p', {'id': build_name + '-ginga-version', 'class' : 'col-md-2 label-base'}, 'No info')
	var zversion = crel('p', {'id': build_name + '-zamba-version', 'class' : 'col-md-2 label-base'}, 'No info')
	var download_href = 'http://172.16.0.206/buildbot/builds//Join-' + build_name + '/LATEST.tar.gz'
	var download = crel('a', {'id': build_name + '-download', 'class' : 'col-md-2 label-base', 'href': download_href}, 'Download')

	return crel('li', {'id': build_name, 'class': 'row'}, name, time, status, gversion, zversion, download)
}

function update_firmware(res) {
	if (res) {
		data = JSON.parse(res)
		join = data['builderName'].replace('Join-', '')

		var st = document.getElementById(join + '-status')
		render_state(data, st)

		document.getElementById(join + '-ginga-version').innerText = find_in(data['properties'], join + '-ginga-version')
		document.getElementById(join + '-zamba-version').innerText = find_in(data['properties'], join + '-zamba-version')
		document.getElementById(join + '-time').innerText = moment.unix(data['times'][0]).format(time_format)
	}
}

function add_to_placeholder(placeholder, tool_name, builds) {
	for (var i = 0; i < builds.length; i++) {
		document.getElementById(placeholder).appendChild(render_build(builds[i]))
		new microAjax("http://172.16.0.206:" + port + "/json/builders/" + builds[i] + "/builds/-1?as_text=1", update_build);
	}
}

function add_release(tool_name, builds) {
	add_to_placeholder('results', tool_name, builds);
}

function add_image(tool_name, builds) {
	add_to_placeholder('images', tool_name, builds);
}

function add_join(join) {
	document.getElementById('joins').appendChild(render_join(join))
	new microAjax("http://172.16.0.206:" + port + "/json/builders/Join-" + join + "/builds/-1?as_text=1", update_firmware);
}

function make_link(builder_name) {
	var href = 'http://172.16.0.206:' + port + '/builders/' + builder_name
	return crel('li', {'class': 'row'}, crel('a', {'id': builder_name + '-name', 'class' : '', 'href': href}, builder_name))
}

function append_item(res) {
	data = JSON.parse(res)
	if (data['text'] != undefined && data['text'][0] == 'failed') {
		document.getElementById('faileds').appendChild(make_link(data['builderName']))
	} else if (data['currentStep'] != undefined) {
		document.getElementById('building').appendChild(make_link(data['builderName']))
	}
}

function get_one(item, index) {
	new microAjax("http://172.16.0.206:" + port + "/json/builders/" + index + "/builds/-1?as_text=1", append_item);
}

function populate_failed(res) {
	if (res) {
		data = JSON.parse(res)
		_.each(data, get_one)
	}
}

window.onload = function() {
	add_release('Ginga', ['GINGA_AML_STATIC_RELEASE', 'GINGA_ANDROID_STATIC_RELEASE', 'GINGA_MT742PV_STATIC_RELEASE', 'GINGA_VS_STATIC_RELEASE', 'GINGA_PPAPI_STATIC_RELEASE', 'GINGA_AML_PPAPI_STATIC_RELEASE'])
	add_release('Zamba', ['ZAMBA_AML_MICROTROL_STATIC_RELEASE', 'ZAMBA_AML_CORADIR_STATIC_RELEASE', 'ZAMBA_MT742PV_STATIC_RELEASE', 'ZAMBA_VS_STATIC_RELEASE'])
	add_release('Wari', ['WARI_ANDROID_STATIC_RELEASE', 'WARI_VS_STATIC_RELEASE'])
	add_release('Testplatform', ['TESTPLATFORM_AML_STATIC_RELEASE', 'TESTPLATFORM_ANDROID_STATIC_RELEASE', 'TESTPLATFORM_MT742PV_STATIC_RELEASE', 'TESTPLATFORM_VS_STATIC_RELEASE'])
	add_release('GingaWrapper', ['GINGAWRAPPER_VS_STATIC_RELEASE'])
	add_release('HTMLShellRemove', ['HTMLSHELLREMOTE_AML_STATIC_RELEASE', 'HTMLSHELLREMOTE_CLANG_64_STATIC_RELEASE'])
	add_release('TVDModule', ['TVDMODULE_AML_STATIC_RELEASE', 'TVDMODULE_CLANG_64_STATIC_RELEASE'])
	add_release('Libcecnode', ['LIBCECNODE_AML_STATIC_RELEASE', 'LIBCECNODE_CLANG_64_STATIC_RELEASE'])

	add_image('TAC', ['TAC_AML_MICROTROL', 'TAC_AML_MICROTROL_WITH_DEPS', 'TAC_AML_MICROTROL_RELEASE_NO_DEPS'])
	add_image('Zamba', ['ZAMBA_IMAGE_AML_MICROTROL', 'ZAMBA_IMAGE_AML_CORADIR'])

	add_join('MT742PV')
	add_join('AML_CORADIR')
	add_join('AML_MICROTROL')

	new microAjax("http://172.16.0.206:" + port + "/json/builders?as_text=1", populate_failed);
};

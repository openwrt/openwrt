/**
 *  Rosy is a theme for LuCI. It is based on luci-theme-bootstrap
 *
 *  luci-theme-rosy
 *     Copyright 2018 Rosy Song <rosysong@rosinson.com>
 *     Copyright 2018 Yan Lan Shen <yanlan.shen@rosinson.com>
 *
 *   Have a bug? Please create an issue here on GitHub!
 *       https://github.com/rosywrt/luci-theme-rosy/issues
 *
 *  luci-theme-bootstrap:
 *      Copyright 2008 Steven Barth <steven@midlink.org>
 *      Copyright 2008 Jo-Philipp Wich <jow@openwrt.org>
 *      Copyright 2012 David Menting <david@nut-bolt.nl>
 *
 *  Licensed to the public under the Apache License 2.0
 */

(function (win, $) {
    $(".loading").fadeOut();

    /**
     * trim text, Remove spaces, wrap
     * @param text
     * @returns {string}
     */
    function trimText(text) {
        return text.replace(/[ \t\n\r]+/g, " ");
    }


    var lastNode = undefined;
    var mainNodeName = undefined;

    var nodeUrl = "";
    (function (node) {
        var luciLocation;
        if (node[0] == "admin") {
            luciLocation = [node[1], node[2]];
        } else {
            luciLocation = node;
        }

        for (var i in luciLocation) {
            nodeUrl += luciLocation[i];
            if (i != luciLocation.length - 1) {
                nodeUrl += "/";
            }
        }
    })(luciLocation);

    /**
     * get the current node by Burl (primary)
     * @returns {boolean} success?
     */
    function getCurrentNodeByUrl() {
        var ret = false;
        if (!$('body').hasClass('logged-in')) {
            luciLocation = ["Main", "Login"];
            return true;
        }

        $(".main-left .nav > .slide > .menu").each(function () {
            var ulNode = $(this);
            ulNode.next().find("a").each(function () {
                var that = $(this);
                var href = that.attr("href");

                if (href.indexOf(nodeUrl) != -1) {
                    ulNode.click();
                    ulNode.next(".slide-menu").stop(true, true);
                    lastNode = that.parent();
                    lastNode.addClass("active");
                    ret = true;
                    return true;
                }
            });
        });
        return ret;
    }

    /**
     * menu click
     */
    if ($('.nav').length > 0 && $(window).width() > 992) {
        var oScroll = new MyScrollBar({
            selId: 'navBox',
            time: 100,
            bgColor: 'transprent',
            barColor: '#839dd67a',
            enterColor: '#839dd6cc',
            enterShow: false
        });
    }
    $(".main-left .nav > .slide > .menu").click(function () {
        var ul = $(this).next(".slide-menu");
        var menu = $(this);
        if (!ul.is(":visible")) {
            menu.addClass("active");
            ul.addClass("active");
            ul.stop(true).slideDown("fast");
        } else {
            ul.stop(true).slideUp("fast", function () {
                menu.removeClass("active");
                ul.removeClass("active");
            });
        }
        if ($('.nav').length > 0 && $(window).width() > 992) {
            oScroll.setSize(200);
        }
        setInterval(function () {
            if ($('.nav').height() < $('.navbar-container').height()) {
                $('.nav').css('transform', 'translate(0px, 0px)');
            }
        }, 300);
        return false;
    });

    /**
     * hook menu click and add the hash
     */
    $(".main-left .nav > .slide > .slide-menu > li > a").click(function () {
        if (lastNode != undefined) lastNode.removeClass("active");
        $(this).parent().addClass("active");
        $(".loading").fadeIn("fast");
        return true;
    });

    /**
     * fix menu click
     */
    $(".main-left .nav > .slide > .slide-menu > li").click(function () {
        if (lastNode != undefined) lastNode.removeClass("active");
        $(this).addClass("active");
        $(".loading").fadeIn("fast");
        win.location = $($(this).find("a")[0]).attr("href");
        return false;
    });

    /**
     * get current node and open it
     */
    if (getCurrentNodeByUrl()) {
        mainNodeName = "node-" + luciLocation[0] + "-" + luciLocation[1];
        mainNodeName = mainNodeName.replace(/[ \t\n\r\/]+/g, "_").toLowerCase();
        $("body").addClass(mainNodeName);
    }
    $(".cbi-button-up").val("");
    $(".cbi-button-down").val("");


    /**
     * hook other "A Label" and add hash to it.
     */
    $("#maincontent > .container").find("a").each(function () {
        var that = $(this);
        var onclick = that.attr("onclick");
        if (onclick == undefined || onclick == "") {
            that.click(function () {
                var href = that.attr("href");
                if (href.indexOf("#") == -1) {
                    $(".loading").fadeIn("fast");
                    return true;
                }
            });
        }
    });

    /**
     * Sidebar expand
     */
    var showSide = false;
    $(".showSide").click(function () {
        if (showSide) {
            $(".darkMask").stop(true).fadeOut("fast");
            $(".main-left").stop(true).fadeOut("fast");
            $(".main-right").css("overflow-y", "visible");
            showSide = false;
        } else {
            $(".darkMask").stop(true).fadeIn("fast");
            $(".main-left").stop(true).animate({
                width: "100%"
            }, "fast").fadeIn("fast");
            $(".main-right").css("overflow-y", "hidden");
            showSide = true;
        }
    });
    if($(win).width() <= 992){
        $(".main-left").click(function(e) {
            e.preventDefault();
            $(".main-left").stop(true).fadeOut("fast");
            showSide = false;
        });
    }


    $(".darkMask").click(function () {
        if (showSide) {
            showSide = false;
            $(".darkMask").stop(true).fadeOut("fast");
            $(".main-left").stop(true).animate({
                width: "0"
            }, "fast");
            $(".main-right").css("overflow-y", "visible");
        }
    });

    $('.login .main-right').css('min-height', $(win).height());
    $(win).resize(function () {
        if ($(win).width() > 921) {
            $(".main-left").css("width", "");
            $(".darkMask").stop(true);
            $(".darkMask").css("display", "none");
            showSide = false;
        }

        $('.login .main-right').css('min-height', $(win).height());
    });

    /**
     * fix legend position
     */
    $("legend").each(function () {
        var that = $(this);
        that.after("<span class='panel-title'>" + that.text() + "</span>");
    });

    $(".cbi-section-table-titles, .cbi-section-table-descr, .cbi-section-descr").each(function () {
        var that = $(this);
        if (that.text().trim() == "") {
            that.css("display", "none");
        }
    });


    $(".main-right").focus();
    $(".main-right").blur();
    $("input").attr("size", "0");

    if (mainNodeName != undefined) {
        console.log(mainNodeName);
        switch (mainNodeName) {
            case "node-status-system_log":
            case "node-status-kernel_log":
                $("#syslog").focus(function () {
                    $("#syslog").blur();
                    $(".main-right").focus();
                    $(".main-right").blur();
                });
                break;
            case "node-status-firewall":
                var button = $(".node-status-firewall > .main fieldset li > a");
                button.addClass("cbi-button cbi-button-reset a-to-btn");
                break;
            case "node-system-reboot":
                var button = $(".node-system-reboot > .main > .main-right p > a");
                button.addClass("cbi-button cbi-input-reset a-to-btn");
                break;
        }
    }

    $('.lang_enInterfaces .ifacebox-head').each(function () {
        $(this).next().css('border-color', $(this).css('background-color'));
    });

    $('<div>').appendTo('.login > .main .cbi-value-last .cbi-value-field');
    $('.login > .main .cbi-value-last .cbi-value-field > div').click(function(){
        var thisParent = $(this).parent();
        if( thisParent.attr('class').indexOf('op-eye') == (-1) ){
            thisParent.addClass('op-eye');
            $(this).prev().attr('type', 'text');
        }else {
            thisParent.removeClass('op-eye');
            $(this).prev().attr('type', 'password');
        }

    });

    $('#cbi-network-switch_vlan .table').wrap('<div id="vlan_table"></div>');
    
})(window, jQuery);

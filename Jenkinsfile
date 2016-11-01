#!/usr/bin/env groovy

/**
 * Build openwrt project
 *
 * Configure as needed, pull in the feeds and run the build. Allows
 * overridding feeds listed using build parameters.
 */
// Be careful with using dir() until JENKINS-33510 is fixed

def customFeeds = [
    ['packages', 'packages', 'https://github.com/CreatorDev'],
    ['ci40', 'ci40-platform-feed', 'https://github.com/CreatorDev'],
    ['creator', 'creator-feed', 'https://github.com/CreatorDev'],
]
def feedParams = []
for (feed in customFeeds) {
    feedParams.add(string(defaultValue: '', description: 'Branch/commmit/PR to override feed. \
        (Can be a PR using PR-< id >)', name: "OVERRIDE_${feed[1].toUpperCase()}"))
}

properties([
    parameters([
        booleanParam(defaultValue: false, description: 'Clean out everything.', \
            name: 'FULL_CLEAN'),
        booleanParam(defaultValue: false, description: 'Build *all* packages for opkg. Warning \
            this will take a very long time!', name: 'ALL_PACKAGES'),
        stringParam(defaultValue: 'target/linux/pistachio/creator-platform-default-cascoda.config', \
            description: 'Config file to use', name: "DEFCONFIG_FILE"),
        stringParam(defaultValue: '', description: 'Set version, if left blank ci build number will be used.', \
            name: 'VERSION'),
    ] + feedParams)
])

node('docker && imgtec') {  // Only run on internal slaves as build takes a lot of resources
    def docker_image
    stage('Prepare') {
        // Clean checkout without wasting bandwidth/time
        sh 'rm -rf .config .config.old feeds.conf.default bin build_dir staging_dir tmp'
        if (env.FULL_CLEAN == 'true'){
            sh 'rm -rf dl'
        }
        checkout scm

        // Default config
        sh "cp ${env.DEFCONFIG_FILE} .config"

        // Versioning
        sh "sed -i 's/.*CONFIG_VERSION_NUMBER.*/CONFIG_VERSION_NUMBER=\"${env.VERSION?.trim() ?: 'j' + env.BUILD_NUMBER}\"/g' .config"
        sh 'scripts/getver.sh > version'

        echo 'Enabling toolchain, image builder and sdk creation'
        sh 'echo \'' \
         + 'CONFIG_MAKE_TOOLCHAIN=y\n' \
         + 'CONFIG_IB=y\n' \
         + 'CONFIG_SDK=y\n' \
         + '\' >> .config'

        // Build all (for opkg)
        // TODO grab vault creds and mod config to use OPKGSMIME
        if (env.ALL_PACKAGES == 'true'){
            echo 'Enabling all user and kernel packages'
            sh 'echo \'' \
             + 'CONFIG_ALL=y\n' \
             + 'CONFIG_ALL_KMODS=y\n' \
             + '\' >> .config'
        }

        // Boardfarm-able
        // TODO work out which ones we actually have
        echo 'Enabling usb ethernet adapters modules (for boardfarm testing)'
        sh 'echo \'' \
         + 'CONFIG_PACKAGE_kmod-usb-net=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-asix=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-cdc-eem=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-cdc-ether=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-cdc-mbim=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-cdc-ncm=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-cdc-subset=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-dm9601-ether=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-hso=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-huawei-cdc-ncm=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-ipheth=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-kalmia=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-kaweth=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-mcs7830=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-pegasus=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-qmi-wwa=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-rndis=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-rtl8150=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-rtl8152=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-sierrawireless=y\n' \
         + 'CONFIG_PACKAGE_kmod-usb-net-smsc95xx=y\n' \
         + '\' >> .config'

        // If specified override each feed with local clone
        for (feed in customFeeds) {
            if (env."OVERRIDE_${feed[1].toUpperCase()}"?.trim()){
                dir("feed-${feed[1]}") {
                    checkout([
                        $class: 'GitSCM',
                        branches: [[name: env."OVERRIDE_${feed[1].toUpperCase()}"]],
                        userRemoteConfigs: [[
                            refspec: '+refs/pull/*/head:refs/remotes/origin/PR-* \
                                +refs/heads/*:refs/remotes/origin/*',
                            url: "${feed[2]}/${feed[1]}.git"
                        ]]
                    ])
                }
                // Replace (or add if not exists) feed with local clone
                sh "grep -q 'src-.* ${feed[0]} .*' feeds.conf.default && \
                    sed -i 's|^.*\\s\\(${feed[0]}\\)\\s.*\$|src-link \\1 ../feed-${feed[1]}|g' feeds.conf.default || \
                    echo 'src-link ${feed[0]} ../feed-${feed[1]}' >> feeds.conf.default"
            }
        }
        sh 'cat .config'
        sh 'cat feeds.conf.default'
        docker_image = docker.image "imgtec/creator-builder:latest" // TODO for now have manually setup on slave
    }
    docker_image.inside {
        stage('Configure') {
            sh 'scripts/feeds update -a && scripts/feeds install -a'
            sh 'make defconfig'
        }
        stage('Build') {
            // Attempt to build quickly and reliably
            // TODO IGNORE_ERRORS=m if build all
            try {
                sh "make -j4 V=s ${env.ALL_PACKAGES ? 'IGNORE_ERRORS=m' : null}"
            } catch (hudson.AbortException err) {
                // BUG JENKINS-28822
                if(err.getMessage().contains('script returned exit code 143')) {
                    throw err
                }
                echo 'Parallel build failed, attempting to continue in  single threaded mode'
            }
            sh "make -j1 V=s ${env.ALL_PACKAGES ? 'IGNORE_ERRORS=m' : null}"
        }

        stage('Upload') {
            archiveArtifacts 'bin/*/*'
        }
    }
}
//node('boardfarm') {
    stage('Intergration test') {
        //TODO run the boardfarm test
    }
//}

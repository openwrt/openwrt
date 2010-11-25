Index: linux-2.6.32.25/drivers/net/cpmac.c
===================================================================
--- linux-2.6.32.25.orig/drivers/net/cpmac.c	2010-11-24 12:16:56.067517245 -0800
+++ linux-2.6.32.25/drivers/net/cpmac.c	2010-11-24 12:17:56.279985384 -0800
@@ -387,6 +387,7 @@
 				    struct cpmac_desc *desc)
 {
 	struct sk_buff *skb, *result = NULL;
+	int offset;
 
 	if (unlikely(netif_msg_hw(priv)))
 		cpmac_dump_desc(priv->dev, desc);
@@ -400,9 +401,13 @@
 
 	skb = netdev_alloc_skb(priv->dev, CPMAC_SKB_SIZE);
 	if (likely(skb)) {
-		skb_reserve(skb, 2);
+		offset = 2;
+		if (priv->phy) {
+			offset += priv->phy->pkt_align;
+		}
+		skb_reserve(skb, offset);
+
 		skb_put(desc->skb, desc->datalen);
-		desc->skb->protocol = eth_type_trans(desc->skb, priv->dev);
 		desc->skb->ip_summed = CHECKSUM_NONE;
 		priv->dev->stats.rx_packets++;
 		priv->dev->stats.rx_bytes += desc->datalen;
@@ -474,7 +479,12 @@
 
 		skb = cpmac_rx_one(priv, desc);
 		if (likely(skb)) {
-			netif_receive_skb(skb);
+			if (priv->phy->netif_receive_skb) {
+				priv->phy->netif_receive_skb(skb);
+			} else {
+				skb->protocol = eth_type_trans(skb, priv->dev);
+				netif_receive_skb(skb);
+			}
 			received++;
 		}
 		desc = desc->next;
@@ -970,7 +980,7 @@
 
 static int cpmac_open(struct net_device *dev)
 {
-	int i, size, res;
+	int i, size, res, offset;
 	struct cpmac_priv *priv = netdev_priv(dev);
 	struct resource *mem;
 	struct cpmac_desc *desc;
@@ -1014,7 +1024,12 @@
 			res = -ENOMEM;
 			goto fail_desc;
 		}
-		skb_reserve(skb, 2);
+		offset = 2;
+		if (priv->phy) {
+			offset += priv->phy->pkt_align;
+		}
+		skb_reserve(skb, offset);
+
 		desc->skb = skb;
 		desc->data_mapping = dma_map_single(&dev->dev, skb->data,
 						    CPMAC_SKB_SIZE,

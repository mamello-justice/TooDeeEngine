import { TooDeeEngine } from '@too-dee-engine/javascript'

export default class BoundingBoxScript implements TooDeeEngine.Script {
    onUpdate(entity: TooDeeEngine.Entity): void {
        const wSize = TooDeeEngine.getRenderTarget().getSize();
        const cTrans = entity.getComponent(TooDeeEngine.ComponentEnum.Transform);
        const cCollider = entity.getComponent(TooDeeEngine.ComponentEnum.BoundingBox);

        // Vertical
        if ((cTrans.pos.y - cCollider.halfSize.y < 0 && cTrans.velocity.y < 0) ||
            (cTrans.pos.y + cCollider.halfSize.y > wSize.y && cTrans.velocity.y > 0)) {
            cTrans.velocity.y *= -1;
        }
        // Horizontal
        if ((cTrans.pos.x - cCollider.halfSize.x < 0 && cTrans.velocity.x < 0) ||
            (cTrans.pos.x + cCollider.halfSize.x > wSize.x && cTrans.velocity.x > 0)) {
            cTrans.velocity.x *= -1;
        }
    }
}
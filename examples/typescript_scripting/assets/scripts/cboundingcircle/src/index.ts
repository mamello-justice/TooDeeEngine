import { TooDeeEngine } from '@too-dee-engine/javascript'

export default class BoundingCircleScript implements TooDeeEngine.Script {
    onUpdate(entity: TooDeeEngine.Entity): void {
        const wSize = TooDeeEngine.getRenderTarget().getSize();
        const cTrans = entity.getComponent(TooDeeEngine.ComponentEnum.Transform);
        const cCollider = entity.getComponent(TooDeeEngine.ComponentEnum.BoundingCircle);

        // Vertical
        if ((cTrans.pos.y - cCollider.radius < 0 && cTrans.velocity.y < 0) ||
            (cTrans.pos.y + cCollider.radius > wSize.y && cTrans.velocity.y > 0)) {
            cTrans.velocity.y *= -1;
        }
        // Horizontal
        if ((cTrans.pos.x - cCollider.radius < 0 && cTrans.velocity.x < 0) ||
            (cTrans.pos.x + cCollider.radius > wSize.x && cTrans.velocity.x > 0)) {
            cTrans.velocity.x *= -1;
        }
    }
}